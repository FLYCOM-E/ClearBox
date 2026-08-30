# Version 5.1.5 (98)

# ADD
- NCDU 기록 자동 정리
- 일본어 지원
- 한국어 지원

# UPDATE
- NCDU 기록 기능을 SQlite3 데이터 저장 방식으로 변경
- 앱 배경화면 모드 최적화
- 다국어 매칭 최적화 및 중복 로직 감소

# FIX
- `settings get system system_locales`가 null을 반환하는 기기에서 언어 대체가 실패하던 문제 수정, 이제 시스템 기본값으로 우선 대체됨
- 일부 메모리 누수 문제 수정
