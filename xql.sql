-- XQL - XML in SQLite
-- Copyright (C) 2020 G. David Butler <gdb@dbSystems.com>
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Affero General Public License as published
-- by the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU Affero General Public License for more details.
--
-- You should have received a copy of the GNU Affero General Public License
-- along with this program.  If not, see <https://www.gnu.org/licenses/>.

-- XQL tag
-- v value
CREATE TABLE IF NOT EXISTS "XqlT"(
 "i" INTEGER PRIMARY KEY
,"v" TEXT NOT NULL CHECK(TYPEOF("v")='text' AND LENGTH("v")>=0)
);
CREATE UNIQUE INDEX IF NOT EXISTS "XqlT_v" ON "XqlT"("v");
CREATE TRIGGER IF NOT EXISTS "XqlT_bd" BEFORE DELETE ON "XqlT" BEGIN
 SELECT RAISE(ABORT,'XqlT referenced') WHERE
     EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(0,OLD."i"))
  OR EXISTS(SELECT * FROM "XqlA" WHERE "n"=OLD."i")
 ;
END;
CREATE TRIGGER IF NOT EXISTS "XqlT_au_i" AFTER UPDATE OF "i" ON "XqlT" WHEN NEW."i"!=OLD."i" BEGIN
 UPDATE "XqlE" SET "v"=NEW."i" WHERE("t", "v")=(0,OLD."i");
 UPDATE "XqlA" SET "n"=NEW."i" WHERE "n"=OLD."i";
END;

-- XQL content
-- v value
CREATE TABLE IF NOT EXISTS "XqlC"(
 "i" INTEGER PRIMARY KEY
,"v" TEXT NOT NULL CHECK(TYPEOF("v")='text' AND LENGTH("v")>=0)
);
CREATE UNIQUE INDEX IF NOT EXISTS "XqlC_v" ON "XqlC"("v");
CREATE TRIGGER IF NOT EXISTS "XqlC_bd" BEFORE DELETE ON "XqlC" BEGIN
 SELECT RAISE(ABORT,'XqlC referenced') WHERE
     EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(1,OLD."i"))
  OR EXISTS(SELECT * FROM "XqlA" WHERE "v"=OLD."i")
 ;
END;
CREATE TRIGGER IF NOT EXISTS "XqlC_au_i" AFTER UPDATE OF "i" ON "XqlC" WHEN NEW."i"!=OLD."i" BEGIN
 UPDATE "XqlE" SET "v"=NEW."i" WHERE("t", "v")=(1,OLD."i");
 UPDATE "XqlA" SET "v"=NEW."i" WHERE "v"=OLD."i";
END;

-- XQL element
-- e is 0 for root else REFERENCES XqlE(i) ON DELETE CASCADE ON UPDATE CASCADE
-- o ordinal
-- t type 0:element 1:content
-- v value REFERENCES t:0:XqlT(i) t:1:XqlC(i) ON DELETE RESTRICT ON UPDATE CASCADE
CREATE TABLE IF NOT EXISTS "XqlE"(
 "i" INTEGER PRIMARY KEY
,"e" INTEGER NOT NULL CHECK(TYPEOF("e")='integer')
,"o" INTEGER NOT NULL DEFAULT 0 CHECK(TYPEOF("o")='integer' AND "o">=0)
,"t" INTEGER NOT NULL CHECK(TYPEOF("t")='integer' AND "t" BETWEEN 0 AND 1)
,"v" INTEGER NOT NULL CHECK(TYPEOF("t")='integer')
);
CREATE UNIQUE INDEX IF NOT EXISTS "XqlE_e_o" ON "XqlE"("e","o");
CREATE INDEX IF NOT EXISTS "XqlE_v_t" ON "XqlE"("v","t");
CREATE TRIGGER IF NOT EXISTS "XqlE_bi" BEFORE INSERT ON "XqlE" BEGIN
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlE.e, not element') WHERE NEW."e"!=0 AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("i","t")=(NEW."e",0));
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlE.v') WHERE
     NEW."t"=0 AND NOT EXISTS(SELECT * FROM "XqlT" WHERE "i"=NEW."v")
  OR NEW."t"=1 AND NOT EXISTS(SELECT * FROM "XqlC" WHERE "i"=NEW."v")
 ;
END;
CREATE TRIGGER IF NOT EXISTS "XqlE_bu_e" BEFORE UPDATE OF "e" ON "XqlE" WHEN NEW."e"!=0 AND NEW."e"!=OLD."e" BEGIN
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed, not element') WHERE NOT EXISTS(SELECT * FROM "XqlE" WHERE("i","t")=(NEW."e",0));
END;
CREATE TRIGGER IF NOT EXISTS "XqlE_bu_t" BEFORE UPDATE OF "t" ON "XqlE" WHEN NEW."t"!=OLD."t" BEGIN
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed, has elements') WHERE EXISTS(SELECT * FROM "XqlE" WHERE "e"=NEW."i");
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlE.v') WHERE
     NEW."t"=0 AND NOT EXISTS(SELECT * FROM "XqlT" WHERE "i"=NEW."v")
  OR NEW."t"=1 AND NOT EXISTS(SELECT * FROM "XqlC" WHERE "i"=NEW."v")
 ;
END;
CREATE TRIGGER IF NOT EXISTS "XqlE_bu_v" BEFORE UPDATE OF "v" ON "XqlE" WHEN NEW."v"!=OLD."v" BEGIN
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlE.v') WHERE
     NEW."t"=0 AND NOT EXISTS(SELECT * FROM "XqlT" WHERE "i"=NEW."v")
  OR NEW."t"=1 AND NOT EXISTS(SELECT * FROM "XqlC" WHERE "i"=NEW."v")
 ;
END;
CREATE TRIGGER IF NOT EXISTS "XqlE_au_i" AFTER UPDATE OF "i" ON "XqlE" WHEN NEW."i"!=OLD."i" BEGIN
 UPDATE "XqlE" SET "e"=NEW."i" WHERE "e"=OLD."i";
 UPDATE "XqlA" SET "e"=NEW."i" WHERE "e"=OLD."i";
END;
CREATE TRIGGER IF NOT EXISTS "XqlE_au_t" AFTER UPDATE OF "t" ON "XqlE" WHEN NEW."t"!=OLD."t" BEGIN
 DELETE FROM "XqlT" WHERE(0,"i")=(OLD."t",OLD."v")
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(OLD."t",OLD."v"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "n"=OLD."n")
 ;
 DELETE FROM "XqlC" WHERE(1,"i")=(OLD."t",OLD."v")
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(OLD."t",OLD."v"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "v"=OLD."v")
 ;
END;
CREATE TRIGGER IF NOT EXISTS "XqlE_au_v" AFTER UPDATE OF "v" ON "XqlE" WHEN NEW."v"!=OLD."v" BEGIN
 DELETE FROM "XqlT" WHERE(0,"i")=(OLD."t",OLD."v")
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(OLD."t",OLD."v"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "n"=OLD."n")
 ;
 DELETE FROM "XqlC" WHERE(1,"i")=(OLD."t",OLD."v")
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(OLD."t",OLD."v"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "v"=OLD."v")
 ;
END;
CREATE TRIGGER IF NOT EXISTS "XqlE_ad" AFTER DELETE ON "XqlE" BEGIN
 DELETE FROM "XqlE" WHERE "e"=OLD."i";
 DELETE FROM "XqlA" WHERE "e"=OLD."i";
 DELETE FROM "XqlT" WHERE(0,"i")=(OLD."t",OLD."v")
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(OLD."t",OLD."v"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "n"=OLD."v")
 ;
 DELETE FROM "XqlC" WHERE(1,"i")=(OLD."t",OLD."v")
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(OLD."t",OLD."v"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "v"=OLD."v")
 ;
END;

-- XQL attribute
-- e element REFERENCES "XqlE"("i") ON DELETE CASCADE ON UPDATE CASCADE
-- o ordinal
-- v value REFERENCES "XqlC"("i") ON DELETE RESTRICT ON UPDATE CASCADE
-- v name REFERENCES "XqlT"("i") ON DELETE RESTRICT ON UPDATE CASCADE
CREATE TABLE IF NOT EXISTS "XqlA"(
 "i" INTEGER PRIMARY KEY
,"e" INTEGER NOT NULL CHECK(TYPEOF("e")='integer')
,"o" INTEGER NOT NULL DEFAULT 0 CHECK(TYPEOF("o")='integer' AND "o">=0)
,"v" INTEGER NOT NULL CHECK(TYPEOF("v")='integer')
,"n" INTEGER CHECK(TYPEOF("n")IN('null','integer'))
);
CREATE UNIQUE INDEX IF NOT EXISTS "XqlA_e_o" ON "XqlA"("e","o");
CREATE INDEX IF NOT EXISTS "XqlA_v" ON "XqlA"("v");
CREATE INDEX IF NOT EXISTS "XqlA_n" ON "XqlA"("n") WHERE "n" IS NOT NULL;
CREATE TRIGGER IF NOT EXISTS "XqlA_bi" BEFORE INSERT ON "XqlA" BEGIN
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlA.e, not element') WHERE NOT EXISTS(SELECT * FROM "XqlE" WHERE("i","t")=(NEW."e",0));
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlE.v') WHERE NOT EXISTS(SELECT * FROM "XqlC" WHERE "i"=NEW."v");
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlE.n') WHERE NEW."n" IS NOT NULL AND NOT EXISTS(SELECT * FROM "XqlT" WHERE "i"=NEW."n");
END;
CREATE TRIGGER IF NOT EXISTS "XqlA_bu_e" BEFORE UPDATE OF "e" ON "XqlA" WHEN NEW."e"!=OLD."e" BEGIN
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlA.e, not element') WHERE NOT EXISTS(SELECT * FROM "XqlE" WHERE("i","t")=(NEW."e",0));
END;
CREATE TRIGGER IF NOT EXISTS "XqlA_bu_v" BEFORE UPDATE OF "v" ON "XqlA" WHEN NEW."v"!=OLD."v" BEGIN
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlE.v') WHERE NOT EXISTS(SELECT * FROM "XqlC" WHERE "i"=NEW."v");
END;
CREATE TRIGGER IF NOT EXISTS "XqlA_bu_n" BEFORE UPDATE OF "n" ON "XqlA" WHEN NEW."n" IS NOT OLD."n" BEGIN
 SELECT RAISE(ABORT,'FOREIGN KEY constraint failed XqlE.n') WHERE NEW."n" IS NOT NULL AND NOT EXISTS(SELECT * FROM "XqlT" WHERE "i"=NEW."n");
END;
CREATE TRIGGER IF NOT EXISTS "XqlA_au_v" AFTER UPDATE OF "v" ON "XqlA" WHEN NEW."v"!=OLD."v" BEGIN
 DELETE FROM "XqlC" WHERE "i"=OLD."v"
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(1,OLD."v"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "v"=OLD."v")
 ;
END;
CREATE TRIGGER IF NOT EXISTS "XqlA_au_n" AFTER UPDATE OF "n" ON "XqlA" WHEN NEW."n" IS NOT OLD."n" BEGIN
 DELETE FROM "XqlT" WHERE OLD."n" IS NOT NULL AND "i"=OLD."n"
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(0,OLD."n"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "n"=OLD."n")
 ;
END;
CREATE TRIGGER IF NOT EXISTS "XqlA_ad" AFTER DELETE ON "XqlA" BEGIN
 DELETE FROM "XqlC" WHERE "i"=OLD."v"
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(1,OLD."v"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "v"=OLD."v")
 ;
 DELETE FROM "XqlT" WHERE OLD."n" IS NOT NULL AND "i"=OLD."n"
  AND NOT EXISTS(SELECT * FROM "XqlE" WHERE("t","v")=(0,OLD."n"))
  AND NOT EXISTS(SELECT * FROM "XqlA" WHERE "n"=OLD."n")
 ;
END;