# MMORPG 서버 포트폴리오

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ff8c094a446f438d98179f8a9dbdae50)](https://www.codacy.com/gh/ahnjm001/MMORPG/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=ahnjm001/MMORPG&amp;utm_campaign=Badge_Grade)

# PPT
[PPT 링크](https://drive.google.com/drive/folders/1v13sMrv2-68Zyx4xrQOSftzltMEuMySs?usp=sharing) 

# 사용 언어
- C++

# 개발 환경
- Windows10
- Visual Studio 2017

# ServerPortfolio폴더 구성
폴더 명 | 설명
---- | ----
_StartExeFiles | 배치파일 묶음 폴더(절대 경로)
DummyClient | 더미 클라이언트
GameDBAgent | MMORPG의 캐릭터 및 몬스터 DB Agent
LoginDBAgent | MMORPG의 로그인 서버의 로그인 계정 DB Agent
LoginServer | MMORPG의 로그인 서버이자 WorldServer
PathFinderAgent | MMORPG의 길찾기 Agent
ZoneServer_WSASend | MMORPG의 존(채널) 서버

# 설명
- 분산형 MMORPG 게임 서버 개발 시 사용이 가능한 라이브러리입니다.
- IOCP를 이용하여 윈도우 환경에서만 사용이 가능합니다.
