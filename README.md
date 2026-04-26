# GTKalc - A GTK4 Calculator

A simple calculator application built with GTK4.

## Author
Harry Elsdon

## Compilation
To compile GTKalc, you need GTK4 development libraries installed.

Compile with:
```
gcc -o gtkalc main.c calculator.c ui.c `pkg-config --cflags --libs gtk4`
```

## Running
```
./gtkalc
```

## Features
- Basic arithmetic operations: +, -, *, /
- Decimal point support
- Clear function
- Nice GTK4 interface with styled display and buttons
