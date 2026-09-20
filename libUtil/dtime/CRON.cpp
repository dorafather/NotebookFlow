#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <ctime>
#include "CRON.h"
namespace nsUtil
{
static std::set<int> parseField(const std::string& field, int minVal, int maxVal)
{
    std::set<int> result;
    std::stringstream ss(field);
    std::string token;
    while (std::getline(ss, token, ',')) {
        int step = 1;
        std::string base = token;
        size_t slashPos = token.find('/');
        if (slashPos != std::string::npos) {
            base = token.substr(0, slashPos);
            step = std::stoi(token.substr(slashPos + 1));
            if (step <= 0) return {};  
        }
        int start, end;
        if (base == "*") {
            start = minVal;
            end = maxVal;
        }
        else if (base.find('-') != std::string::npos) {
            size_t dashPos = base.find('-');
            start = std::stoi(base.substr(0, dashPos));
            end = std::stoi(base.substr(dashPos + 1));
        }
        else {
            start = end = std::stoi(base);
        }
        if (start < minVal || end > maxVal || start > end) return {};
        for (int v = start; v <= end; v += step) {
            result.insert(v);
        }
    }
    return result;
}
static int getDayOfWeek(int year, int month, int day)
{
    std::tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;
    t.tm_hour = 12;  
    std::mktime(&t); 
    return t.tm_wday;
}
CheckSched::mon::mon(){m_pOwner=NULL;}
CheckSched::mon::~mon(){}
void CheckSched::mon::INIT(CheckSched * _pOwner){m_pOwner= _pOwner;}
void CheckSched::mon::PROC()
{
	Iterator grpItr;
	while(true)
	{
		m_groupLock.RLOCK();
		jobGroup* pGrp = (jobGroup*)m_groups.next(grpItr);
		if(!pGrp)
		{
			m_groupLock.UNLOCK();
			break;
		}
		pGrp->m_lock.RLOCK();
		Iterator jobItr;
		job* pJob = (job*)pGrp->m_jobs.next(jobItr);
		while(pJob)
		{
			TIME curT;
			if(isMatch((KCSTR)pJob->m_cronStmt, curT))
				if(m_pOwner) m_pOwner->JOB(*pJob);
			pJob = (job*)pGrp->m_jobs.next(jobItr);
		}
		pGrp->m_lock.UNLOCK();
		m_groupLock.UNLOCK();
		g_fnMilliSleep(10);
	}
	g_fnMilliSleep(10);
}
void CheckSched::mon::REG(KCSTR _cronStmt, RestMsg & _msg)
{
	job* pNew = new job;
	pNew->setkey((KCSTR)_msg.GET("id").VAL());
	pNew->m_cronStmt = _cronStmt;
	pNew->m_data = _msg;

	m_groupLock.RLOCK();
	Iterator itr;
	jobGroup* pTarget = NULL;
	jobGroup* pGrp = (jobGroup*)m_groups.next(itr);
	while(pGrp)
	{
		if(pGrp->m_jobs.size() < 100)
		{
			pTarget = pGrp;
			break;
		}
		pGrp = (jobGroup*)m_groups.next(itr);
	}
	m_groupLock.UNLOCK();
	if(pTarget)
	{
		pTarget->m_lock.WLOCK();
		pTarget->m_jobs.pushback(pNew);
		pTarget->m_lock.UNLOCK();
	}
	else
	{
		jobGroup* pNewGrp = new jobGroup;
		pNewGrp->m_lock.WLOCK();
		pNewGrp->m_jobs.pushback(pNew);
		pNewGrp->m_lock.UNLOCK();
		m_groupLock.WLOCK();
		m_groups.pushback(pNewGrp);
		m_groupLock.UNLOCK();
	}
}
void CheckSched::mon::DEL(KCSTR _id)
{
	m_groupLock.RLOCK();
	Iterator grpItr;
	jobGroup* pGrp = (jobGroup*)m_groups.next(grpItr);
	while(pGrp)
	{
		pGrp->m_lock.WLOCK();
		job* pJob = (job*)pGrp->m_jobs.find(_id);
		if(pJob)
		{
			pGrp->m_jobs.del(_id);
			pGrp->m_lock.UNLOCK();
			m_groupLock.UNLOCK();
			return;
		}
		pGrp->m_lock.UNLOCK();
		pGrp = (jobGroup*)m_groups.next(grpItr);
	}
	m_groupLock.UNLOCK();
}

CheckSched::CheckSched(){m_pMonitor=NULL;}
CheckSched::~CheckSched()
{
	if(m_pMonitor) delete m_pMonitor;
}
void CheckSched::INIT()
{
	m_pMonitor = new mon;
	m_pMonitor->RUN("cronChk");
}
void CheckSched::REG(KCSTR _cronStmt, RestMsg & _msg)
{
	if(m_pMonitor)
		m_pMonitor->REG(_cronStmt, _msg);
}
void CheckSched::DEL(RestMsg & _msg)
{
	if(m_pMonitor)
		m_pMonitor->DEL((KCSTR)_msg.GET("id").VAL());
}
bool CheckSched::isMatch_(KCSTR _cron,
                  unsigned int _year, unsigned int _mon, unsigned int _day,
                  unsigned int _hour, unsigned int _mins)
{
	std::string cron; cron = _cron;
	std::vector<std::string> fields;
	std::stringstream ss(cron);
	std::string field;
	while (ss >> field) 
	{
		fields.push_back(field);
	}
	if (fields.size() != 5) return false;
	std::set<int> minutes  = parseField(fields[0],  0, 59);
	std::set<int> hours    = parseField(fields[1],  0, 23);
	std::set<int> days     = parseField(fields[2],  1, 31);
	std::set<int> months   = parseField(fields[3],  1, 12);
	std::set<int> weekdays = parseField(fields[4],  0, 6);
	if (minutes.empty() || hours.empty() || days.empty() ||
	months.empty() || weekdays.empty()) 
	{
		return false;
	}
	if (minutes.count(_mins) == 0)  return false;
	if (hours.count(_hour) == 0)    return false;
	if (months.count(_mon) == 0)    return false;
	bool dayIsWildcard = (fields[2] == "*");
	bool weekdayIsWildcard = (fields[4] == "*");
	int wday = getDayOfWeek(_year, _mon, _day);
	bool dayMatch = days.count(_day) > 0;
	bool weekdayMatch = weekdays.count(wday) > 0;
	if (dayIsWildcard && weekdayIsWildcard) 
	{
		return true;  
	}
	else if (!dayIsWildcard && !weekdayIsWildcard) 
	{
		return dayMatch || weekdayMatch;  
	}
	else 
	{
		return dayMatch && weekdayMatch; 
	}
}
bool CheckSched::isMatch(KCSTR _cron, TIME & _time)
{
	return isMatch_(_cron,
					_time.Year(),
					_time.Mon(),
					_time.Day(),
					_time.Hour(),
					_time.Min());
}
void CheckSched::checkCronTest()
{
    std::cout << isMatch_("0 * * * *", 2026, 5, 14, 15, 0) << std::endl;  // 1
    std::cout << isMatch_("0 * * * *", 2026, 5, 14, 15, 30) << std::endl; // 0
    std::cout << isMatch_("0 3 * * *", 2026, 5, 14, 3, 0) << std::endl;   // 1
    std::cout << isMatch_("*/15 * * * *", 2026, 5, 14, 10, 30) << std::endl; // 1
    std::cout << isMatch_("*/15 * * * *", 2026, 5, 14, 10, 31) << std::endl; // 0
    std::cout << isMatch_("0 9 * * 1-5", 2026, 5, 14, 9, 0) << std::endl;  // 1
    std::cout << isMatch_("0 9 * * 1-5", 2026, 5, 16, 9, 0) << std::endl;  // 0
    std::cout << isMatch_("0 12 1,15 * *", 2026, 5, 15, 12, 0) << std::endl; // 1
    std::cout << isMatch_("0 9,12,18 * * *", 2026, 5, 14, 12, 0) << std::endl; // 1
}
}