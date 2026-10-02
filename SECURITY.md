# Security Policy

This library transmits on a vehicle bus that controls lights, locks and windows. Security reports are therefore taken seriously.

## Supported Versions

Security fixes are applied to the latest code on the `main` branch and to the most recent release.

## Reporting a Vulnerability

If you find a security issue, **please do not open a public issue**.

Instead, email **muksin.muksin04@gmail.com** with:

- A description of the issue and its potential impact
- Steps to reproduce (board, transceiver, sketch)
- A suggested fix, if you have one

You will receive a response as soon as possible, and credit in the release notes if you wish.

## Notes for Users

- The library sends whatever your sketch asks it to send. If your project exposes that over Wi-Fi, Bluetooth or another network, protecting that interface is the job of your project.
- Test with the vehicle stationary. A message that is harmless on one model can do something else on another.
