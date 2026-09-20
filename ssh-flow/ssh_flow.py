from fastapi import FastAPI, HTTPException
from pydantic import BaseModel, Field
from typing import Optional
import paramiko
import socket

app = FastAPI(title="SSH Command API", version="1.0")

class SSHRequest(BaseModel):
    host: str = Field(..., example="203.0.113.10")
    port: int = Field(10022, example=10022)
    username: str = Field(..., example="deploy_user")
    password: Optional[str] = Field(None, example="your-ssh-password")
    private_key: Optional[str] = Field(None, description="PEM 형식의 private key 내용 (password 대신 사용)")
    command: str = Field(..., example="ls -la /tmp")
    timeout: int = Field(30, description="명령 실행 타임아웃(초)")

class SSHResponse(BaseModel):
    success: bool
    exit_code: Optional[int] = None
    stdout: str = ""
    stderr: str = ""
    error: Optional[str] = None

@app.post("/notebookflow/ssh/execute", response_model=SSHResponse)
def execute_ssh_command(req: SSHRequest):
    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())  # 실제 운영에서는 known_hosts 검증 권장

    try:
        # 인증 방식 선택
        if req.private_key:
            # 문자열로 받은 private key를 메모리에서 로드
            from io import StringIO
            key_file = StringIO(req.private_key)
            pkey = paramiko.RSAKey.from_private_key(key_file)
            client.connect(
                hostname=req.host,
                port=req.port,
                username=req.username,
                pkey=pkey,
                timeout=10,
                allow_agent=False,
                look_for_keys=False
            )
        elif req.password:
            client.connect(
                hostname=req.host,
                port=req.port,
                username=req.username,
                password=req.password,
                timeout=10,
                allow_agent=False,
                look_for_keys=False
            )
        else:
            raise HTTPException(status_code=400, detail="password 또는 private_key 중 하나는 필수입니다.")

        # 명령 실행
        stdin, stdout, stderr = client.exec_command(req.command, timeout=req.timeout)

        exit_code = stdout.channel.recv_exit_status()
        out = stdout.read().decode(errors="replace")
        err = stderr.read().decode(errors="replace")

        return SSHResponse(
            success=(exit_code == 0),
            exit_code=exit_code,
            stdout=out,
            stderr=err
        )

    except paramiko.AuthenticationException:
        raise HTTPException(status_code=401, detail="SSH 인증 실패 (아이디/비밀번호 또는 키 확인)")
    except paramiko.SSHException as e:
        raise HTTPException(status_code=500, detail=f"SSH 오류: {str(e)}")
    except socket.timeout:
        raise HTTPException(status_code=504, detail="연결 또는 명령 실행 타임아웃")
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"알 수 없는 오류: {str(e)}")
    finally:
        client.close()

@app.get("/")
def root():
    return {"message": "SSH Command API is running. Use POST /execute"}


# PyInstaller로 빌드된 배포용 단일 exe는 --reload를 쓸 수 없다(리로더가
# 자기 자신을 서브프로세스로 다시 spawn하려 하는데 exe에는 그 스크립트가
# 없음) - 그래서 main()을 직접 호출한다. 개발 중에는 start.ps1의
# `uvicorn ssh_flow:app --reload`로 띄운다.
def main():
    import uvicorn
    uvicorn.run(app, host="127.0.0.1", port=8450, log_level="info")


if __name__ == "__main__":
    main()