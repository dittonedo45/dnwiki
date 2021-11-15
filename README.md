# dnwiki (Ditto Nedo's Wiki)

dnwiki is an open source software (wikipedia api client) for linux. It is based on the [wikipedia](https://www.wikipedia.com) site api. It necessitates streaming a wikipedia page text to other terminal programs.

#Examples

	dnwiki download -p ./Cw -s Monopoly
	dnwiki reader -l # To List Downloaded Pages
	dnwiki reader -l -p . # To  List Downloaded Pages from current working directory
	dnwiki reader -p ./Cw  -T Monopoly # Converts the Downloaded page of Title _Monopoly_ to text
	dnwiki reader -p ./Cw  -T Monopoly | espeak -vrob | play -qtwav - # Reads the Page 'Monopoly'
	dnwiki reader -p ./Cw  -T Bargain | less # Views The Page Bargain

#Libraries Required

	libsox
	libjq
	libcurl

#Installing
	make
