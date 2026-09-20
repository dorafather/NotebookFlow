/*
  
{
  "SCE_EVENT": "START_REQ",
  "ACTION_EVENT": "event_0",
  "AS_ID": 99,
  "PARAMS": 
  {
    "id": 1,
    "caller": "01012345678",
    "called": "01099998888",
    "SCE_NAME": "CPH_test",
	"input_rows":
	[
		{
			"SUB_ID": "2",
			"NAME": "김철수",
			"MSISDN": "01087654321",
			"COLORING_YN": "1",
			"SVC_TYPE": "3"
		},
		{
			"SUB_ID": "3",
			"NAME": "이영희",
			"MSISDN": "01011112222",
			"COLORING_YN": "0",
			"SVC_TYPE": "0"

		},
		{
			"SUB_ID": "4",
			"NAME": "박지성",
			"MSISDN": "01033334444",
			"COLORING_YN": "1",
			"SVC_TYPE": "1",
			"CONTENT_ID": "CONT_POP_001"
		},
		{
			"SUB_ID": "5",
			"NAME": "손흥민",
			"MSISDN": "01055556666",
			"COLORING_YN": "1",
			"SVC_TYPE": "1",
			"CONTENT_ID": "CONT_JAZZ_001"
		},
		{
			"SUB_ID": "6",
			"NAME": "김연아",
			"MSISDN": "01077778888",
			"COLORING_YN": "1",
			"SVC_TYPE": "1",
			"CONTENT_ID": "CONT_ROCK_001"
		}
	]
  }
}
*/

SCE::NAME
{
  CPH_test
}
STATE::INIT
{
  START   PROC.pCphInit
  event_0 PROC.pCphInit
  TERM    PROC.pTerm
}
STATE::STOP
{
  TERM    PROC.pTerm
}
PROC::pCphInit
{
  IF(TRUE)
    ACT_SET.setInit
    PROC.pCphBuild
}
PROC::pCphBuild
{
  IF(TRUE)
    FUNC.CPH(name_list,RCV_MSG.PARAMS.input_rows,1)
    FUNC.CPH(msisdn_list,RCV_DATA.input_rows,2)
    FUNC.CPH(coloring_list,RCV_MSG.PARAMS.input_rows,3)
    PROC.pCphResult
}
PROC::pRsp
{
	IF(SESSION.LIST.name_list[4] == 김연아)
		ACT_SEND.sndRsp
		GOTO.STOP
}
ACT_SET::setInit
{
  SESSION.caller = RCV_DATA.caller
}
PROC::pCphResult
{
  IF(TRUE)
    LOG.info(name_0 = SESSION.LIST.name_list[0])
    LOG.info(name_1 = SESSION.LIST.name_list[1])
    LOG.info(name_2 = SESSION.LIST.name_list[2])
    LOG.info(name_3 = SESSION.LIST.name_list[3])
    LOG.info(name_4 = SESSION.LIST.name_list[4])
    LOG.info(msisdn_0 = SESSION.LIST.msisdn_list[0])
    LOG.info(msisdn_4 = SESSION.LIST.msisdn_list[4])
    LOG.info(coloring_0 = SESSION.LIST.coloring_list[0])
    LOG.info(coloring_1 = SESSION.LIST.coloring_list[1])
    PROC.pRsp
}
ACT_SEND::sndRsp
{
	SND_HDR.TYPE = test_echo
	SND_HDR.name2 = SESSION.LIST.name_list[2]
	SND_HDR.msisdn4 = SESSION.LIST.msisdn_list[4]
}
PROC::pTerm
{
  IF(TRUE)
    LOG.info(Finaly Terminated!)
}
