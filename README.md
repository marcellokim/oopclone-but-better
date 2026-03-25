# oopclone-but-better

실시간 2D 영토전 전략 게임 프로토타입입니다.  
C++20 + SFML 3 기반으로 제작되었고, 4개 국가의 비대칭 능력치와 AI를 바탕으로 타일형 전장에서 수도 함락 또는 전군 소멸을 목표로 싸웁니다.

## 현재 상태
- 플레이 가능한 프로토타입
- 4개 국가 선택 가능
- 실시간 클릭 이동 / 공격
- 수도 리젠 / 영토 비례 강화
- AI 상대 3개 국가 동시 진행
- 단계별 UI 리디자인 및 phase-2 polish 반영 완료

## 핵심 특징
- **4개 국가 비대칭**
  - 기동 / 공격 / 방어 / 리젠 성향이 다름
- **실시간 타일 전장**
  - 내 타일 선택 → 아군/적 타일 클릭으로 명령
- **수도 기반 승패 구조**
  - 수도 함락 또는 전체 병력 소멸 시 패배
- **결정론적 시뮬레이션 코어**
  - 테스트 가능한 fixed-tick 구조
- **SFML 기반 전술형 UI**
  - nation select / 전장 HUD / debrief 화면 포함

## 조작법
- **좌클릭**: 타일 선택 / 명령
- **우클릭**: 선택 해제
- **Esc**: 선택 해제
- **1 / 2 / 3**: 병력 전송 비율 25% / 50% / 100%
- **Enter 또는 좌클릭**: 결과 화면에서 nation select로 복귀

## 빌드
요구 사항:
- C++20 지원 컴파일러
- CMake 3.24+
- GitHub 접근 가능 네트워크 환경 (SFML을 `FetchContent`로 받아옴)

```bash
cmake -S . -B build
cmake --build build -j4 --target territory_war territory_war_tests
```

## 실행
```bash
./build/territory_war
```

## 테스트
```bash
ctest --test-dir build --output-on-failure
```

## 오프스크린 프리뷰 렌더
UI 점검용 preview 이미지를 생성할 수 있습니다.

```bash
cmake --build build -j4 --target territory_war_preview
./build/territory_war_preview
```

생성 위치:
- `.omx/screens/preview-nation-select.png`
- `.omx/screens/preview-match.png`
- `.omx/screens/preview-game-over.png`

## 폰트
현재 UI는 번들된 재배포 가능 폰트를 사용합니다.
- Display: IBM Plex Serif
- Body: Lato
- Mono: IBM Plex Mono

관련 파일:
- `assets/fonts/README.md`
- `docs/ui-style-guide.md`

## 주요 디렉터리
- `src/` — 애플리케이션 / 시뮬레이션 / UI 구현
- `include/` — 헤더
- `tests/` — 회귀 테스트
- `assets/fonts/` — 번들 폰트
- `docs/` — UI 스타일 / 플레이테스트 / 검증 문서
- `.omx/plans/` — 작업 계획 산출물

## 문서
- `docs/manual-smoke-checklist.md`
- `docs/playtest-ui-balance-checklist.md`
- `docs/ui-style-guide.md`
- `docs/combined-ui-upgrades-phase2-verification.md`

## 주의 사항
- 현재는 프로토타입 단계이며, **실제 사람 손으로 하는 최종 플레이테스트**는 추가로 권장됩니다.
- SFML은 `FetchContent` 기반으로 내려받습니다.
