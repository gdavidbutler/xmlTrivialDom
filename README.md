# xmlTrivialDom
Trivial XML DOM in SQLite

### XML

My [xmlTrivialCallbackParser](https://github.com/gdavidbutler/xmlTrivialCallbackParser) enables fast and easy XML document parsing and processing.
The callback style works well as a driving flow, in other words, when all the support to process an XML document can be invoked as parsed.
However, many times, an XML document is needed to provide support for other flows.
In this case, the document must be searchable and/or modifiable.

### Database.

Using a SQL database for a DOM has many advantages.
The biggest one is the ability to use a standard query language, [SQL](https://en.wikipedia.org/wiki/SQL).

Using [SQLite](https://sqlite.org) for a DOM has many advantages.
The biggest one is it supports small, fast and embeddable databases.

### XQL

Find the API in xql.h:

* xqlSchema
  * Create an XQL schema on a connection, if it is missing
* xml2xql
  * Parse an XML document into an XQL schema rooted at a specified element
* xql2xml
  * Generate an XML document from an XQL schema at a specified element

### Example

* test/main.c: read an XML document(s) on standard input, adding to an optional SQLite "database", then output XML document(s) from the "database".

### Building

Edit Makefile providing locations for SQLite and my XML parser.
Use "make" or review the file "Makefile" to build.
