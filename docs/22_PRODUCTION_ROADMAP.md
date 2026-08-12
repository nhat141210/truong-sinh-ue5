# Lộ trình sản xuất

## Cách dùng

Lộ trình theo cổng chất lượng, không theo số tuần. Không được sang mốc tiếp theo chỉ vì UI/level nhìn đẹp. 27_IMPLEMENTATION_STATUS.md là trạng thái sống; cập nhật sau mỗi build, test, donor gate hoặc quyết định kiến trúc.

## M0 — Chuẩn hóa và môi trường Windows

Đầu ra: repository có commit baseline, handoff docs, exact UE5.8 build/Visual
Studio/Git LFS trên Windows, reference vault cục bộ và Unreal MCP smoke test.
Donor chỉ inventory metadata ở M0; không clone/import trước native project build.

Gate: agent mới clone repo đọc được `00_START_HERE.md`, chạy environment check;
không có reference asset/text trong Git; exact engine build được ghi; source control
sạch sau commit. Chưa tuyên bố project chạy nếu chưa có build/Editor evidence.

## M1 — Architectural proof

Đầu ra: UE5.8 C++ project sạch; module descriptor/DAG; `L_Dev_Smoke`; Enhanced
Input; player third-person; Blueprint interaction bridge; typed command/event;
composition facade; deterministic RNG v1; Content Registry; save schema v1 và
CommonUI shell.

Gate: Development Editor, Development Game và Shipping build/package được trên
Windows; PIE + Standalone spawn đúng pawn trong `L_Dev_Smoke`; New Game/Continue
round-trip hash; RNG golden vector pass; đi trong zone không tăng game date;
simulation test chạy không cần level 3D.

## M2A — Golden loop gameplay canonical

Đầu ra: graybox một zone, một NPC, một quest ngắn, một travel action, một lần tu
luyện, một đột phá tối thiểu và một trận combat theo lượt. Tất cả đi qua command →
result → event → save; không dùng Blueprint rule giả.

Gate: New Game → interaction → time action → combat → save → Continue cho cùng
state hash với cùng seed; animation skip không đổi result; không dead-end. Chỉ cần
graybox đọc được, chưa phải final art.

## M2B — Visual target corridor final-quality

Đầu ra: một corridor/không gian thành thị nhỏ final-quality, một nhân vật/NPC,
Lumen/Nanite/material/Niagara/animation, UI shell và chu kỳ ánh sáng mẫu. Nó dùng
golden loop M2A, không có gameplay riêng trong Level Blueprint.

Gate: i5-12400F + RTX 3060, 1080p High mục tiêu 60 FPS và 1% low từ 45 FPS sau
warm-up; Lumen Overview/Mesh Distance Fields/Nanite visualization được review;
visual review pass; High/Medium scalability có số đo.

## M3 — Core parity mở rộng

Đầu ra: chronology, action-time cost, tuổi thọ, cảnh giới, ngộ đạo, đột phá, seeded RNG, NPC/world tick và save/load deterministic.

Gate: cùng snapshot/seed luôn cho cùng world result; reload không đổi ngày, NPC state, random state hay vị trí player; mỗi rule tham chiếu audit có parity entry hoặc UNKNOWN.

## M4 — Combat parity và 3D presentation mở rộng

Đầu ra: combat snapshot, ngũ hành, linh khí, công pháp, thần thông, equipment, effect, AI, replay log, arena presentation, skip/fast-forward.

Gate: golden scenario so được từng lượt/kết quả; skip animation không đổi result/save; presentation failure không corrupt simulation.

## M5 — Sản xuất tu tiên

Đầu ra: tu luyện/bế quan, luyện đan, luyện khí, động phủ, linh điền, pháp bảo, thiên kiếp và interaction 3D tương ứng.

Gate: recipe, action cost và outcome có oracle; failure, thiếu điều kiện và load giữa chuỗi hành động được test.

## M6 — Thế giới, xã hội và kinh tế

Đầu ra: tông môn, quest, cống hiến, NPC lifecycle/relationship, thành thị/cửa hàng, đấu giá, chạy thương, location travel, bí cảnh và kỳ ngộ.

Gate: chuyển zone áp đúng time cost; NPC simulation sau nhiều tháng/năm vẫn deterministic; không dead quest, soft-lock hay actor presentation sai state sau load.

## M7 — Content parity

Đầu ra: content catalog có kiểm chứng: NPC, quest, item, technique, recipe, sect, location, event và story progression.

Gate: mọi entry có stable ID, localization key, validator pass, route kiểm thử; không stub ở main progression.

## M8 — Full run tới phi thăng

Đầu ra: nhân vật mới đi hết cảnh giới, route tông môn/tán tu, bí cảnh, đại sự kiện, thiên kiếp và ending/phi thăng.

Gate: tối thiểu hai full-run route độc lập hoàn tất; save migration và replay test pass ở late-game snapshot.

## M9 — Parity, UX và performance QA

Đầu ra: parity matrix, test report, screenshot baseline, profiling report và lỗi được triage.

Gate: toàn bộ mục parity bắt buộc PASS, không UNKNOWN/placeholder trên đường chính, High/Medium/Low đạt budget và clean-machine QA không blocker.

## M10 — Windows release

Đầu ra: Shipping x64 package, README người chơi, changelog, crash/log collection cục bộ tùy chọn và manifest build.

Gate: package chạy trên máy sạch không có Unreal/Epic Launcher; New Game, Continue, combat, save/load và một route endgame smoke test pass.

## Công việc lặp

- Mỗi task: đọc status → branch/checkpoint → implement → build/automation → PIE nếu cần → update docs/status → commit.
- Mọi đổi content: chạy validator và test reference liên quan.
- Mọi đổi 3D: capture screenshot + stat unit/stat gpu; dùng Insights/memory report khi vượt budget.
- Mọi donor/plugin: qua cổng 21_DONOR_REUSE_MAP.md.

## Không làm sớm

Không seamless world, multiplayer, Steam/EOS, runtime online service hoặc action
combat. Không mass-import asset/content trước M2A và visual target M2B pass. Không
bắt đầu full story/content trước M3/M4 ổn định. Không clone donor trước khi native
M1 build pass; donor không được trở thành đường bắt buộc để boot project.
