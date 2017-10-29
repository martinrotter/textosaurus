#!/bin/bash

# Transka executable.
TRANSKA=./transka

# Get credentials.
read -p "Username: " USERNAME
read -p "Password: " PASSWORD

# Setup parameters.
RESOURCE=../../../localization/textilosaurus_en.ts
CODES="en_GB"
TRANSLATION='../../../localization/textilosaurus_$CODE.ts'

declare PARAMS

PARAMS+="-u "$USERNAME" -p "$PASSWORD" -ps "textilosaurus" -rs "textilosaurus" -ur "$RESOURCE" "

for CODE in $CODES; do
  PARAMS+="-dt "$CODE" "$(eval echo $TRANSLATION)" "
done

cd ./transka

$TRANSKA $PARAMS