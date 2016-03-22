#!/bin/sh

./texflow MFT | sed "s/_/\\\\_/g" | sed "s/</$<$/g" | sed "s/>/$>$/g" > reference.tex
