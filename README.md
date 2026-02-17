# OCRWSS-epita

## Sunmary
IA OCR Text recognition written from scratch in C, paired with a word search solver, image processing filters, and a graphical interface

Made with a team of 4 fellow students

## Build instructions
### Dependencies
- GTK
- SDL2
- make
- clang
- pkg-config

In repository root:

```make```

## Technical stack
- C
- GTK

## Details
The model is trained by using a backpropagation algorithm written from scratch in C

Inputs can be defined using a csv file and trained models are saved in a custom .model format (mainly saving weights but also network configuration)

Letter detection in the image is done using algorithmic pattern recognition.
