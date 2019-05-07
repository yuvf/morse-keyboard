### Morse Key USB Keyboard (Hebrew/English)

#### What's this?

This repo contains code to convert a physical Morse key into a USB keyboard, via an Arduino with a supported processor - ATmega32U4.
This means you can plug the Morse key into any computer and use it in exactly the same way as a conventional keyboard, except you type in Morse and the characters appear on screen.

The keyboard currently supports Hebrew, English lowercase letters and numbers 0-9.

Timings between characters and words are currently hardcoded in ms. This isn't hugely accurate since different operators will key at different speeds, but the hardcoded timings can easily be tweaked.

#### What do I need to replicate this?

You'll need an Arduino with ATmega32U4 processor and a physical Morse key. I've configured my Arduino so the output from the Morse key is connected to Pin 11.

#### Added sequences
"......" - Enter
"......." - Backspace
"........" - Change language
