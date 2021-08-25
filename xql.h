/*
 * XQL - XML in SQLite
 * Copyright (C) 2020 G. David Butler <gdb@dbSystems.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* create an XQL schema in con: tables XqlT, XqlC, XqlE and XqlA */
/* return sqlite3_exec() result code */
int xqlSchema(
  sqlite3 *con
);

/* truncate tables XqlT, XqlC, XqlE and XqlA */
/* return sqlite3 result code */
int xqlTruncate(
  sqlite3 *con
);

/* parse an XML document of len, limiting max depth, with(out) white bodies into an XQL schema at element (0 for document) in con */
/* return -sqlite3_errorcode on error else offset of last char parsed */
/* the database is updated with all that was parseable */
int xml2xql(
  sqlite3 *con
 ,sqlite3_int64 element
 ,const unsigned char *xml
 ,unsigned int len
 ,unsigned int max
 ,int whiteBody
);

/* generate XML document from con starting at element (0 for all documents) */
/* return 0 on error else sqlite3_malloc'd zero terminated XML */
/* if return is not 0 and len is not 0, length is written to len */
char *xql2xml(
  sqlite3 *con
 ,sqlite3_int64 element
 ,unsigned int *len
);
