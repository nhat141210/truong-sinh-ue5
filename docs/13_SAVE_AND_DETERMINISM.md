# Save và determinism

## Save v2

V2 lưu world seed, elapsed minutes, exploration remainder, world revision/layer, soul, current vessel, RNG streams, committed command IDs và pending replay. Khi thêm NPC/event/home/economy, các domain snapshot đi vào cùng versioned envelope.

JSON codec v2 hiện có ở source; atomic replace, backup và migration file cần Windows filesystem test trước khi gọi Continue hoàn chỉnh.

## Hash

Canonical state được sort theo stable ID/GUID và hash BLAKE3 với prefix version. Presentation, actor transform trang trí, animation time và graphics settings không nằm trong canonical hash.

## RNG

- SplitMix64 versioned.
- Stream ID lowercase theo domain + CommandId.
- Preview không tiêu RNG.
- Confirm/resolve/reload cùng CommandId không reroll.
- Không seed từ clock, frame, pointer, map actor order hoặc display text.

## Pending replay

Outcome commit trước cinematic. Save ghi replay ID/plan reference; Continue có thể phát tiếp hoặc show summary. Clearing replay không apply reward lần nữa.

## File safety cần triển khai Windows

```text
serialize + hash
→ write .tmp
→ flush/close
→ validate .tmp
→ rotate current to .bak
→ atomic replace
```

Load order: current → backup → báo lỗi tiếng Việt và New Game. Không im lặng ghi đè save hỏng.

## Không offline progress

Không lưu wall-clock để cộng thời gian khi mở lại. Game date chỉ đổi qua committed commands.
