#!/bin/bash
#

readonly NAME="cs6360_project_1"
readonly RELEASE_FOLDER="${HOME}/${NAME}"
readonly RELEASE_ZIP="${HOME}/${NAME}.zip"

# delete previous release zip
if [ -f "$RELEASE_ZIP" ]; then
  rm "$RELEASE_ZIP"
fi

mkdir -p "$RELEASE_FOLDER"/src
# copy source files
cp -ar library_management/* "$RELEASE_FOLDER"/src
# copy readme.txt
cp readme.txt "$RELEASE_FOLDER"
# copy data sources
cp -ar data_source/ "$RELEASE_FOLDER"
# compile quick_start_guide.tex
pushd documents/guide > /dev/null 2>&1
pdflatex -output-directory="$RELEASE_FOLDER" quick_start_guide.tex > /dev/null 2>&1
popd > /dev/null 2>&1
# compile design.tex
pushd documents/design > /dev/null 2>&1
pdflatex -output-directory="$RELEASE_FOLDER" design.tex > /dev/null 2>&1
popd > /dev/null 2>&1
# clean auxiliary files
pushd "$RELEASE_FOLDER" > /dev/null 2>&1
rm *.aux *.log *.out
popd > /dev/null 2>&1
# package all files
pushd "${HOME}" > /dev/null 2>&1
zip -r "$RELEASE_ZIP" "$NAME"/*
chmod 777 "$RELEASE_ZIP"
popd > /dev/null 2>&1

# delete release folder
if [ -d "$RELEASE_FOLDER" ]; then
  rm -rf "$RELEASE_FOLDER"
fi
