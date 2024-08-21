#!/bin/bash

IMG_DIR="sandbox/"
JPG_FILE_1="$IMG_DIR""trash/*.jpg"
JPG_FILE_2="$IMG_DIR""trash/*.JPG"
RAW_FILE="$IMG_DIR""trash/*.ARW"
PIC_DIR="$IMG_DIR"
RAW_DIR="$IMG_DIR"

mv $JPG_FILE_1 $PIC_DIR
mv $JPG_FILE_2 $PIC_DIR
mv $RAW_FILE $RAW_DIR
