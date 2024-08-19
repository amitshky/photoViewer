#!/bin/bash

JPG_FILE_1="test/trash/*.jpg"
JPG_FILE_2="test/trash/*.JPG"
RAW_FILE="test/trash/*.ARW"
PIC_DIR="test/pic/"
RAW_DIR="test/raw/"

mv $JPG_FILE_1 $PIC_DIR
mv $JPG_FILE_2 $PIC_DIR
mv $RAW_FILE $RAW_DIR
