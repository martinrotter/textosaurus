#!/bin/bash

# Transka executable.
TRANSKA=./transka

# Get credentials.
read -p "Username: " USERNAME
read -p "Password: " PASSWORD

# Setup parameters.
RESOURCE=../../../localization/scipio_en.ts
CODES="en_GB"
TRANSLATION='../../../localization/scipio_$CODE.ts'

declare PARAMS

PARAMS+="-u "$USERNAME" -p "$PASSWORD" -ps "scipio" -rs "scipio" -ur "$RESOURCE" "

for CODE in $CODES; do
  PARAMS+="-dt "$CODE" "$(eval echo $TRANSLATION)" "
done

cd ./transka

$TRANSKA $PARAMS