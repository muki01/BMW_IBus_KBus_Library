# Contributing to the BMW I-Bus / K-Bus Library

Thank you for taking the time to contribute! Every test report, bug report, fix and idea makes this library better for everyone who works on a classic BMW.

By participating, you agree to follow the [Code of Conduct](CODE_OF_CONDUCT.md).

## Ways to Contribute

| | |
|---|---|
| 🧪 **Report a tested platform** | The ESP32, STM32, Raspberry Pi Pico and UNO R4 builds are experimental. Tell us how they behave in a car. |
| 🚗 **Report a tested vehicle** | Which chassis and model year did you try, and what worked? |
| 🐛 **Report a bug** | Use the **Bug report** template and include the debug output whenever possible. |
| 💡 **Suggest a feature** | Use the **Feature request** template. |
| 🔧 **Submit code** | Fixes, board support, new examples. |
| 📝 **Improve the docs** | Clearer explanations, wiring photos, corrections. |

## Reporting Bugs

Before opening an issue, please search the [existing issues](https://github.com/muki01/BMW_IBus_KBus_Library/issues). A good report includes:

- The library version or commit
- The board (e.g. Arduino Nano, ESP32 DevKit) and the Arduino core version
- The transceiver (TH3122.4, ELMOS 10026B, MCP2025, optocouplers)
- The vehicle: chassis and model year
- A minimal sketch that shows the problem
- **The debug output** from `setIbusDebug()`. The `Good Message -> …`, `Message Bad -> …` and `TRANSMITING CODE: …` lines are the most useful information.

## Development Workflow

1. **Fork** the repository and create a branch from `main`:
   ```bash
   git checkout -b feature/my-improvement
   ```
2. Make your changes, keeping them **focused**: one fix or feature per pull request.
3. **Test in a car** when your change touches receiving, transmitting or timing, and say in the pull request which board and vehicle you tested on.
4. Make sure all examples still **compile**. The same check runs automatically on every pull request, for the Arduino Nano and the ESP32.
5. Commit with a clear message, e.g. `Add timeout to the transmit queue`.
6. Push and open a **pull request** against `main`, filling in the template.

## Coding Guidelines

- Follow the existing style of the file you are editing: naming, indentation and comment density.
- **Do not change the Timer2 code path for AVR boards without testing it in a car.** The bus-idle timing decides whether a message collides with the car's own traffic.
- Put platform-specific code behind preprocessor guards, as the existing code does.
- Wrap constant debug strings in `F()` to save RAM on AVR boards.
- New public methods need an entry in `keywords.txt` and in the API table of the README.
- Car-specific messages do not belong in the library; they live in the [firmware project](https://github.com/muki01/BMW_IBus_KBus).

## License

By contributing, you agree that your contributions will be licensed under the [MIT License](LICENSE).
