# Public Release Checklist

이 문서는 현재 private 저장소를 public으로 전환하기 전에 확인할 최소 체크리스트입니다.

## 이미 정리된 항목
- 내부 OMX 작업 상태/계획/context 산출물은 git 추적 대상에서 제외
- `.idea/`, `build/`, `cmake-build-*`, `docs/playtest-runs/` 등 로컬/생성 산출물 제외
- README를 포트폴리오/리뷰어 기준의 영어 안내로 정리 완료
- macOS / Windows portable ZIP 패키징 스크립트 추가 완료
- `LICENSE` 포함 완료
- 실제 플레이테스트 시작 스크립트 추가 완료

## public 전환 전에 확인할 것
1. **README 최종 문구 확인**
   - 외부 사용자가 봐도 이해 가능한 프로젝트 설명인지
   - 빌드/실행/테스트 방법이 최신인지
2. **폰트/자산 라이선스 재확인**
   - `assets/fonts/`에 포함된 폰트가 재배포 가능한지 재검토
3. **실제 사람 기준 플레이테스트 1회 이상 수행**
   - `./tools/start_playtest.sh`
   - `docs/playtest-ui-balance-checklist.md` 작성
4. **GitHub 메타데이터 설정**
   - repository description
   - topics
   - optional: homepage / releases / screenshots
5. **첫 portable release ZIP 업로드**
   - macOS: `./tools/package_portable.sh --test`
   - Windows: `.\tools\package_portable.ps1 -Test`

## public 전환 커맨드 예시
```bash
gh repo edit marcellokim/oopclone-but-better --visibility public
```

## public 전환 후 권장 작업
- README에 실제 스크린샷 추가
- 첫 release tag 작성
- issues / project board / roadmap 정리
