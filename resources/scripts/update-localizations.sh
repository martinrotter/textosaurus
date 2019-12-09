#!/bin/bash

# Transka executable.
TRANSKA=./transka

# Get credentials.
read -p "Username: " USERNAME
read -p "Password: " PASSWORD

# Setup parameters.
RESOURCE=../../../localization/textosaurus_en.ts
CODES="cs de es fr sk uk_UA zh_CN"
TRANSLATION='../../../localization/textosaurus_$CODE.ts'

declare PARAMS

PARAMS+="-u "$USERNAME" -p "$PASSWORD" -ps "textosaurus" -rs "textosaurus" -ur "$RESOURCE" "

for CODE in $CODES; do
  PARAMS+="-dt "$CODE" "$(eval echo $TRANSLATION)" "
done

cd ./transka

$TRANSKA $PARAMS