# uSDXOpen
Open firmware for all versions of the uSDX transceiver
Branched from https://github.com/threeme3/usdx  Copyright 2019, 2020, 2021   Guido PE1NNZ <pe1nnz@amsat.org>
Credits: Guido PE1NNZ, Dan Tayloe N7VE (http://norcalqrp.org/files/Tayloe_mixer_x3a.pdf), DL2MAN, and many others
FB Groups: "usdx usdr radios", "usdx", "(tr)usdx Open"

Aim: To develop and support the uSDX firmware for all common versions of uSDX from (tr)uSDX to the variety of common built models.
Improvements: The first improvement was to resolve distortion occuring on SSB TX audio caused by quadrature flipping and other issues.
Following that, the code was ammended to track the mode (USB, LSB, CW) and frequency last used on each band, also to allow the menu to cycle rather than forcing users to go all the way back and forth, and to enable by-directional band changes by changing the frrequency up or down, and then double clicking the rotary encoder to change band in the desired direction, which with 8 band units was needed.
Other changes such as easy model configuration, custom call-sign display on the LCD, etc., are detailed on the ino file itself.
We have also developed a full DSP add-on module with touch LCD and wide-band spectrum running off Dan Tayloe's sensitive Quadrature mixer.
We're also looking at developing a voice interface (voice recognition and text to speech responses) for hands-free operation.
Enjoy using this firmware, constructive criticism always welcome!
73, Rob GW8RDI

Use of this firmware is at the user's own risk. No warranty is provided although effort will be given where available to help all users.

Legal basis for the distribution of modified open source firware based on the original license conditions:
1. Constitutional right of artistic expression. 2. Consumer law rights to distribute self-modified products, hardware and own or open-source modified software, and share that information openly.
Original license terms:-
"// QCX-SSB.ino - https://github.com/threeme3/QCX-SSB
// Copyright 2019, 2020, 2021 Guido PE1NNZ <pe1nnz@amsat.org>
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
"
Reproduced from the original project:-

uSDX is a simple and experimental (Class-E driven) SSB and CW SDR transceiver. It can be used to make QRP SSB contacts, or (in combination with a PC) used for the digital modes such as FT8, JS8, FT4. It can be fully-continuous tuned through bands 80m-10m in the LSB/USB-modes with a 2400Hz bandwidth has up to 5W PEP SSB output and features a software-based full Break-In VOX for fast RX/TX switching in voice and digital operations.

The SSB transmit-stage is implemented entirely in digital and software-based manner: at the heart the ATMEGA328P is sampling the input-audio and reconstructing a SSB-signal by controlling the SI5351 PLL phase (through tiny frequency changes over 800kbit/s I2C) and controlling the PA Power (through PWM on the key-shaping circuit). In this way a highly power-efficient class-E driven SSB-signal can be realized; a PWM driven class-E design keeps the SSB transceiver simple, tiny, cool, power-efficient and low-cost (ie. no need for power-inefficient and complex linear amplifier with bulky heat-sink as often is seen in SSB transceivers).

For the receiver, most parts are implemented in digital manner (software): the ATMEGA328P is implementing a 90 degree phase shift circuit, the (CW/SSB) filter circuit and the audio amplifier circuit (now a class-D amplifier). This has simplifies the uSDX circuit a lot, and there are a number of advantages and features (compared to an analog approach): there is no longer a need for an I/Q alignment procedure due to the very accurate 90 degree Hilbert phase shifter; and there are now adjustable IF DSP filters for CW and SSB; and there is an AGC and there is a noise-reducing DSP signal conditioning function and there are three indepent built-in attenuators in the analog front-end which helps utilizing the full dynamic range. The speaker is directly connected and driven by the ATMEGA. A digital mixer with narrow low-pass window (2 kHz), steep roll-off (-45dB/decade) combined with an oversampling and decimating ADC are responsible for a processing gain, dynamic range and alias rejection sufficient to handle weak and strong signal conditions (e.g. contests or listening on 40m just next to broadcasting band).

This experiment is created to try out what is can be achieved with minimal hardware while moving complexity towards software; here the approach followed is to simplify the design where possible while keep a reasonable performance. The result is a cheap, easy to build, versatile QRP SSB transceiver that actually is quite suitable for making QSOs (even in contest situations), however due to the experimental nature some parts are still in progress and hence limited. Feel free to try it out or to experiment with this sketch, let me know your thoughts or contribute here: https://github.com/threeme3/usdx

Note: there is an active online forum discussing the uSDX here: uSDX Forum.

73, Guido pe1nnz@amsat.org
