pandoc -s ..\CHANGES.md -o ..\bin\changes.html --css=Ipponboard.css --self-contained
pandoc -s ..\LICENSE.md -o ..\bin\license.html --css=Ipponboard.css --self-contained

pandoc -s anleitung.md -o ..\bin\Anleitung.html --toc --css=Ipponboard.css --number-sections --self-contained

