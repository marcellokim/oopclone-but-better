# Public Release Checklist

이 문서는 현재 private 저장소를 public으로 전환하기 전에 확인할 최소 체크리스트입니다.

## 이미 정리된 항목
- 내부 OMX 작업 상태/계획/context 산출물은 git 추적 대상에서 제외
- `.idea/`, `build/`, `cmake-build-*`, `docs/playtest-runs/` 등 로컬/생성 산출물 제외
- README 정리 완료
- 실제 플레이테스트 시작 스크립트 추가 완료

## public 전환 전에 확인할 것
1. **README 최종 문구 확인**
   - 외부 사용자가 봐도 이해 가능한 프로젝트 설명인지
   - 빌드/실행/테스트 방법이 최신인지
2. **LICENSE 추가 여부 결정**
   - public 전환 전에는 라이선스를 명시하는 것이 좋음
3. **폰트/자산 라이선스 재확인**
   - `assets/fonts/`에 포함된 폰트가 재배포 가능한지 재검토
4. **실제 사람 기준 플레이테스트 1회 이상 수행**
   - `./tools/start_playtest.sh`
   - `docs/playtest-ui-balance-checklist.md` 작성
5. **GitHub 메타데이터 설정**
   - repository description
   - topics
   - optional: homepage / releases / screenshots

## public 전환 커맨드 예시
```bash
gh repo edit marcellokim/oopclone-but-better --visibility public
```

## public 전환 후 권장 작업
- README에 실제 스크린샷 추가
- 첫 release tag 작성
- issues / project board / roadmap 정리
