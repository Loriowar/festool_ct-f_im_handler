### Overview
An alternative to microcontroller usage based on a hard logic.

### Inspiration
- [Similar hardware implementation](https://github.com/jenskueper/festool-ctl-sys-bluetooth/issues/1). But without a real implementation.
- [More elegant hardware solution](https://youtu.be/EyrakKOR5tI?t=174). It uses clock from a FT receiver instead of data output.

#### Scheme
![hard_logic_scheme](https://github.com/Loriowar/festool_ct-f_im_handler/assets/2118479/d5f1c74b-2363-484d-86ea-5e99c1c1fa06)

#### How it works?
It contains two logical parts: monoflop and T-trigger.

Monoflop produces a single positive pulse. It "smooths" all digital pulses and transform it into one pulse to switch T-triggeer.

After power-on T-trigger always switch to high state (Vcc on Q of U1.1) and wait a pulse on C. It has an additional chain (C2, R3) to add a switch delay.
The chain has to prevent redundant switches within short period of time (~100ms) if more than one pulse came in.

Monoflop wait any signal on S. After the signal it switchwes to a high state. The duration of the pulse is adjusted by RC-chain (C3, R6) and has ~300ms.

In most cases, such configuration is enough to transform square digital pulses into a single switch of T-trigger and subsequent switch of a relay.

#### Nuances
The scheme was checked on breadboard anf found following artefacts.
1. After power-on (~500ms) Festool receiver produces a pulse. It switches the relay. This leads to the following border case: after power-on the vacuum (or any other load) will start and almost immediately stop.
2. In case of rapid push on the green FT button (delay between pushes less than 3sec) you can see multiple switches. I.e. the state of the load after several subsequent pushes is not defined and will be random.
