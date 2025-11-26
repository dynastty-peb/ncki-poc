# 🕵️‍♂️ Non-Content Keystroke Inference  #project #PoC #sidechannel

**(Research‑oriented PoC • User‑mode timing side channel)**  #research #security

## 👀 Overview  #overview #intro

Non-Content Keystroke Inference is a **defensive research PoC** demonstrating how much sensitive context can leak from **pure timing metadata** — without capturing content, injecting code, or requiring elevated privileges.

The core idea is simple but surprisingly powerful:
even an **ordinary user‑mode process** can observe low-level keyboard timing and infer *what kind of data* a user is entering — like structured numeric sequences (payment cards 💳), short verification codes 🔢, or password-like bursts 🔑.

The goal is to highlight how seemingly harmless OS APIs can unintentionally expose meaningful side-channel signals.

---

## ⚠️ Important Disclaimer  #disclaimer #ethics

This project is published **strictly for educational, academic, and defensive‑security purposes**.

It is **not** meant for monitoring real users, collecting sensitive info, or performing unauthorized surveillance.
Operational or weaponizable components are deliberately omitted.

Any misuse is **solely the responsibility of the user**.

---

## 💡 Motivation  #motivation #context

Modern operating systems provide APIs for subscribing to low-level keyboard events. Legitimate uses include accessibility tools, automation utilities, and hotkey managers, but they also expose **high-resolution timing metadata**.

This PoC demonstrates that:

* Runs fully in **user mode**  #user-mode
* Requires **no elevated privileges**  #privilege
* Uses **no injection techniques**  #injection
* Never reads window contents  #privacy
* Does **not** function as a keylogger  #keylogger

### 📝 Technical Note on Character Processing  #technical #notes

This PoC internally processes printable characters **only** to distinguish numeric bursts from mixed-character bursts for classification purposes.
It does **not** store, log, transmit, or reconstruct typed content.
Character data is used exclusively for non-content, structure-only research analysis.

---

## 🧩 Key Concept  #concept #technical

* **Burst segmentation:** pauses longer than ~900ms separate independent “typing sessions” ⏱️  #burst
* **Character composition:** numeric-only vs. mixed sequences 🔢🔡  #composition
* **Length heuristics:** numeric sequences match predictable lengths (e.g., cards vs. MFA codes)  #heuristics
* **Timing metadata:** burst duration and cadence hint at structured vs. free-form input  #timing

This PoC does **not** attempt to recover actual secrets.
It demonstrates that “non-content” metadata alone can reveal *the type* of secret being entered — a subtle but real risk.

---

## ✅ What This PoC Shows (and What It Does NOT)  #scope #limitations

### ✔️ Demonstrated  #demonstrated

* Real-time classification of bursts by timing and structure ⏱️  #realtime
* Numeric sequences naturally clustering into predictable ranges 🔢  #numeric
* Detection of correction behavior via Backspace timing ⌫  #backspace
* Exposure of contextual signals via standard, non-privileged APIs  #api

### ❌ Not Included / Intentionally Omitted  #omitted

* No keylogging or content capture ❌  #keylogger
* No persistence, stealth, or injection ❌  #stealth
* No unauthorized monitoring ❌  #monitoring
* No exploitation or deployment instructions ❌  #exploit
* No data exfiltration logic ❌  #exfil

This is a **research artifact**, not a usable attack tool.

---

## 📚 Background & Related Work  #background #research

Prior work explored timing attacks in SSH, JavaScript, acoustic channels, and masked typing — mainly focusing on **recovering content**.

This project instead explores a **non-content side channel**:
inferring *the category* of sensitive input purely from timing patterns.

Relevant in environments requiring strong confidentiality and process isolation.

---

## 🤝 Responsible Disclosure Position  #responsible #disclosure

This repository helps:

* OS vendors evaluate API risks 🖥️  #vendor
* Developers implement countermeasures (jitter, obfuscation) 🛡️  #defense
* Security teams detect suspicious low-level hook activity 🔍  #monitoring
* Researchers study input side channels 📊  #research

Vendors or security teams can open an issue or reach out for more context.

---

## 🛡️ Defensive Recommendations  #defense #recommendations

* Restrict or virtualize low-level keyboard hooks in untrusted contexts  #sandbox
* Add artificial jitter or noise to inter-keystroke timing  #jitter
* Monitor processes requesting global keyboard hooks  #monitor
* Use keystroke obfuscation or decoy frameworks  #obfuscation
* Treat timing metadata as sensitive, not incidental  #sensitive

---

## 🎓 Academic Value  #academic #value

Useful for:

* Teaching side-channel fundamentals 🧠  #education
* Demonstrating OS-level metadata leakage paths  #metadata
* Comparing timing inference with other leakage vectors  #comparison
* Evaluating keystroke obfuscation measures  #evaluation
* Raising awareness in defensive security communities  #community

---

## ⚖️ Ethical Use  #ethics

By accessing or using this repository, you agree to use it **only** for research, education, and legitimate security analysis.
Unauthorized use against individuals, systems, or organizations is strictly prohibited.

---

## 📄 License  #license

This project is released under the **MIT License**.  
Ethical-use restrictions still apply as described in this repository — the PoC is for **research, educational, and defensive security purposes only**.

---

*For responsible disclosure discussions, collaboration, or questions, open a GitHub issue or contact the maintainer.*  #contact #disclosure
