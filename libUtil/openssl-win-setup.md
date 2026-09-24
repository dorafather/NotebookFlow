# OpenSSL(정적, MT) 빌드 — NotebookFlow.exe TLS 검증용

`NotebookFlow.exe`는 Windows OS 인증서 저장소(WinHTTP/Schannel)에 의존하지
않고, 자체 번들 CA 인증서(`NotebookFlow/cacert.pem`)로 TLS 서버 인증서를
검증합니다(2026-09-25 도입 — 오래되거나 방치된 Windows에서 OS 인증서
저장소가 낡아 있어도 항상 동작하게 하려는 목적, `OutboundClient.cpp` 참고).
이를 위해 vendored `httplib.h`를 `CPPHTTPLIB_OPENSSL_SUPPORT`로 빌드하며,
OpenSSL 정적 라이브러리(`/MT`, 이 프로젝트의 CRT 링키지와 일치)가 필요합니다.

`libUtil/openssl-win/`(`.gitignore`로 제외됨 — 다른 빌드 산출물과 동일한
정책)에 아래 셋업을 한 번 실행해 두면 `NotebookFlow\1. build_NotebookFlowt.bat`가
그 경로의 include/lib를 그대로 사용합니다.

## ⚠️ 반드시 한글이 없는 경로에서 빌드할 것

OpenSSL의 어셈블리(NASM) 빌드 단계가 `-g`(디버그 정보) 옵션으로 소스
파일을 해시하는데, 이 프로젝트 저장소 경로(`...\개인자료\DSL\...`)처럼
한글이 포함된 경로에서 실행하면 `unable to hash file` 오류로 실패합니다
(이 저장소에서 이미 `asif`/`nats.c` 빌드 때도 겪은 것과 같은 계열의
한글 경로 함정 — `asif_windows_static_lib`/`nats_c_windows_build` 메모리
참고). **반드시 `C:\` 바로 아래 같은 순수 ASCII 경로에서 vcpkg install을
실행**하고, 빌드가 끝난 결과물만 이 저장소 안으로 복사해오세요.

## 절차 (1회만 실행하면 됨)

```powershell
# 1. 한글 없는 임시 작업 폴더 생성
mkdir C:\vcpkg-work-nf
Set-Content -Path C:\vcpkg-work-nf\vcpkg.json -Value @'
{
  "name": "notebookflow-openssl",
  "version": "1.0.0",
  "builtin-baseline": "e5a1490e1409d175932ef6014519e9ae149ddb7c",
  "dependencies": ["openssl"]
}
'@

# 2. Visual Studio Build Tools에 번들된 vcpkg로 빌드(소스 컴파일이라 10분+ 걸림)
#    builtin-baseline 값은 이 VS 설치의 vcpkg-bundle.json의 embeddedsha와
#    맞춰야 함(다른 VS 버전이면 그 파일에서 값을 다시 확인).
$vcpkg = "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\vcpkg\vcpkg.exe"
Push-Location C:\vcpkg-work-nf
& $vcpkg install --triplet x64-windows-static
Pop-Location

# 3. 결과물을 이 저장소로 복사(경로는 실제 저장소 위치에 맞게 조정)
$dst = "<이 저장소 경로>\libUtil\openssl-win"
New-Item -ItemType Directory -Force "$dst\include", "$dst\lib" | Out-Null
Copy-Item C:\vcpkg-work-nf\vcpkg_installed\x64-windows-static\include\* "$dst\include" -Recurse -Force
Copy-Item C:\vcpkg-work-nf\vcpkg_installed\x64-windows-static\lib\libssl.lib "$dst\lib" -Force
Copy-Item C:\vcpkg-work-nf\vcpkg_installed\x64-windows-static\lib\libcrypto.lib "$dst\lib" -Force
```

빌드 스크립트가 참조하는 최종 결과물:

```
libUtil/openssl-win/include/openssl/*.h
libUtil/openssl-win/lib/libssl.lib
libUtil/openssl-win/lib/libcrypto.lib
```

## 링크 시 필요한 추가 Windows 시스템 라이브러리

`vcpkg_installed/.../lib/pkgconfig/libcrypto.pc`의 `Libs:` 필드 기준
(OpenSSL 3.6.1, x64-windows-static): `crypt32.lib`, `ws2_32.lib`,
`advapi32.lib`, `user32.lib`. `NotebookFlow\1. build_NotebookFlowt.bat`의
link 커맨드에 이미 반영되어 있습니다.
