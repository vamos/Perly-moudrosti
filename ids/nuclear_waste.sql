--WORKING STUFF

CREATE SEQUENCE fix_seq START WITH 1;
CREATE OR REPLACE TRIGGER  on_null_fix_id
  BEFORE INSERT ON Fix
  FOR EACH ROW
BEGIN
  IF :NEW.fid IS NULL THEN
    :NEW.fId := fix_seq.nextval;
  END IF;
END;
/

DROP TRIGGER on_null_fix_id;
DROP SEQUENCE fix_seq;

CREATE OR REPLACE PROCEDURE bad_blood AS
BEGIN
  DELETE
  FROM Donor
  WHERE EXISTS (SELECT dId FROM Test
                WHERE Test.dId = Donor.Id AND Test.result = 1
                GROUP BY dId
                HAVING sum(result) >= 3);
END;
/

-- TESTING "GARBAGE"


CREATE OR REPLACE TRIGGER  three_bad
  AFTER INSERT ON Test
  FOR EACH ROW
  DECLARE
    test_count  INTEGER;      --# of depts for this employee
    max_tests   INTEGER := 3; --max number of depts per employee.
BEGIN

  IF (test_count >= max_tests) THEN
    --DBMS_OUTPUT.PUT_LINE('Loop number');
      RAISE_APPLICATION_ERROR (-20000,'MAX 3 FAILED TESTS, NO MORE INV');
  ELSE
    NULL;

  END IF;
END;
/




CREATE OR REPLACE TRIGGER  no_more_invitation
  BEFORE INSERT ON Invitation
  FOR EACH ROW
  DECLARE
    test_count  INTEGER;      --# of depts for this employee
    max_tests   INTEGER := 3; --max number of depts per employee.
BEGIN
  SELECT COUNT(*) INTO test_count
  FROM Test
  WHERE Test.dId = :NEW.dId AND Test.result = 1;


  --IF (test_count > max_tests) THEN
    DBMS_OUTPUT.PUT_LINE('Loop number');
      --RAISE_APPLICATION_ERROR (-20000,'MAX 3 FAILED TESTS, NO MORE INV');
  --ELSE
  --  NULL;

  --END IF;
END;
/

DROP TRIGGER on_null_fix_id;


DECLARE
test_count INTEGER;
BEGIN
SELECT COUNT(*) INTO AS test_count
FROM Test
WHERE Test.dId = 2;
END;

CREATE OR REPLACE TRIGGER only_two_good
   BEFORE UPDATE OR INSERT ON Test
   FOR EACH ROW
 DECLARE
   test_count  INTEGER;      --# of depts for this employee
   max_tests   INTEGER := 3; --max number of depts per employee.
 BEGIN
   SELECT COUNT(*) INTO test_count
   FROM Test
   WHERE Test.dId = :NEW.dId;


  IF test_count > max_tests THEN
      RAISE_APPLICATION_ERROR (-20000,'MAX 2 FAILED TESTS');
  ELSE
    NULL;

  END IF;
END;
 /

CREATE OR REPLACE TRIGGER only_two_good
   BEFORE UPDATE OR INSERT ON Test
   FOR EACH ROW
 DECLARE
   test_count  INTEGER;      --# of depts for this employee
   max_tests   INTEGER := 3; --max number of depts per employee.
 BEGIN
   SELECT COUNT(*) INTO test_count
    FROM Test
   WHERE Test.dId = :NEW.dId;

  IF :OLD.dId = :NEW.dId THEN
    RETURN;
  ELSE
    IF test_count >= max_tests THEN
      RAISE_APPLICATION_ERROR (-20000,'MAX 2 FAILED TESTS');
END IF;
  END IF;
END;
 /


CREATE OR REPLACE TRIGGER
AFTER INSERT OR UPDATE
ON Test
REFERENCING NEW AS NEW OLD AS OLD
BEGIN

MERGE INTO Test Merged_test
using
(
SELECT Donor.Id ,COUNT(merged_test.result) test_am
    FROM Donor
    join
    Test
    on Donor.id = Test.dId
    group by Donor.id
)    merged_donors
on (Test.dId = merged_donors.Id)
when matched then
update set Merged_test.test_am = merged_donors.test_am;
END;
/

CREATE OR REPLACE TRIGGER  delete_on_three_fails
  BEFORE INSERT ON TEST
  --FOR EACH ROW
BEGIN
    --SELECT DISTINCT Test.dId, COUNT (*) AS failed_tests
    --FROM Test ON Test.dId=:NEW.dId AND Test.result = 1;
    --GROUP BY Test.dId;
-- for these cases delete not allowed
   IF
   THEN
    --DELETE FROM Donor WHERE Donor.id = :NEW.dId ON CASCADE
  END IF;
END;
/




SELECT *
FROM Customer
WHERE name = 'Jan Novak';

DROP INDEX client_sname;

EXPLAIN PLAN FOR SELECT sname, COUNT(*) AS TOTAL_FIXs_ORDERED
FROM Client JOIN Fix ON Client.Id=Fix.cId
GROUP BY Client.sname;
--ORDER BY TOTAL_FIXs_ORDERED DESC;
SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());

CREATE INDEX client_sname
  ON Client (sname);

EXPLAIN PLAN FOR SELECT /*+ INDEX(Client client_sname)*/ sname, COUNT(*) AS TOTAL_FIXs_ORDERED
FROM Client JOIN Fix ON Client.Id=Fix.cId
GROUP BY Client.sname;
--ORDER BY TOTAL_FIXs_ORDERED DESC;
SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());





EXPLAIN PLAN FOR SELECT DISTINCT pid, title, fname, sname
FROM Employee JOIN Test ON Employee.id = Test.eid
WHERE "date"= DATE '2019-01-15';
SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());



CREATE OR REPLACE TRIGGER  delete_on_tree_fails
  AFTER INSERT ON Test
  FOR EACH ROW
BEGIN
  IF SELECT COUNT(:NEW.dId) IS 3 THEN
    DELETE FROM Donor WHERE Donor.id = :NEW.dId
  END IF;
END;
/

DROP TRIGGER delete_on_tree_fails;

DELIMITER $$
CREATE TRIGGER bad_blood AFTER INSERT ON Test
FOR EACH ROW
BEGIN
  IF SELECT :new.dId
  DELETE FROM Donor where Id = NEW.id ON CASCADE;
END $$


-- all of blood  1 fix = 500ml
SELECT SUM(pool)/2 "Total Blood Pool (liters)"
FROM Branch;

SELECT *
FROM Client;
SELECT *
FROM Employee;
SELECT *
FROM Donor;
SELECT *
FROM Branch;
SELECT *
FROM Fix;
SELECT *
FROM Collection;

select *
FROM FIX natural join COLLECTION;

-- How many clients do we have?
SELECT COUNT(*)
FROM client;

-- Which clients have taken their doses in Coronary St?
SELECT DISTINCT C.title, C.fname, C.sname
FROM CLIENT C , FIX F
WHERE C.id=F.cid AND F.branchid='Coronary St';

-- Which employees have met with Bloody Mary?
SELECT DISTINCT E.title, E.fname, E.sname
FROM EMPLOYEE E, TEST T, DONOR D
WHERE E.id=T.eid AND D.id=T.did AND D.fname='Mary' AND D.sname='Bloody';

-- INSTRUCTIONS FOR SELECTS
-- 2x spojeni dvou tabulek
-- 1x spojeni tri tabulek
-- 2x GROUP BY + agregacni fce
-- 1x EXISTS
-- 1x IN s vnorenym selectem
-- jaka data hleda, jaka je jeho funkce


SELECT Donor.fname, Donor.sname
FROM Donor
WHERE Id IN ( SELECT dId
              FROM Invitation
              WHERE "date" BETWEEN DATE '2019-01-01' AND DATE '2019-12-31');


CREATE OR REPLACE TRIGGER Branch_on_update_cascade
  AFTER UPDATE OF name
  ON Branch
  FOR EACH ROW
  BEGIN
    UPDATE Account SET branch = :NEW.name WHERE branch = :OLD.name;
  END;
/

CREATE OR REPLACE TRIGGER Print_salary_changes
  BEFORE DELETE OR INSERT OR UPDATE ON Emp_tab
  FOR EACH ROW
WHEN (new.Empno > 0)
DECLARE
    sal_diff number;
BEGIN
    sal_diff  := :new.sal  - :old.sal;
    dbms_output.put('Old salary: ' || :old.sal);
    dbms_output.put('  New salary: ' || :new.sal);
    dbms_output.put_line('  Difference ' || sal_diff);
END;
/


CREATE OR REPLACE TRIGGER update_blood_pool
  BEFORE DELETE OR INSERT OR UPDATE ON Fix
  FOR EACH ROW
WHEN (new.Empno > 0)
DECLARE
    sal_diff number;
BEGIN
    sal_diff  := :new.sal  - :old.sal;
    dbms_output.put('Old salary: ' || :old.sal);
    dbms_output.put('  New salary: ' || :new.sal);
    dbms_output.put_line('  Difference ' || sal_diff);
END;
/


GRANT SELECT ON Fix TO xvodak05;

GRANT CREATE MATERIALIZED VIEW TO xvodak05
GRANT CREATE DATABASE LINK TO xvodak05;

CREATE DATABASE LINK DB1.WORLD CONNECT TO scott IDENTIFIED BY tiger USING 'DB1.WORLD';
CREATE DATABASE LINK local
   CONNECT TO hr IDENTIFIED BY hr
   USING 'local';
CREATE MATERIALIZED VIEW emp_mv
BUILD IMMEDIATE
REFRESH FORCE
ON DEMAND
AS
SELECT * FROM emp@db1.world;




CREATE OR REPLACE TRIGGER trg
BEFORE INSERT
ON t1
FOR EACH ROW

DECLARE
 val nr_of_fixes_stored;
BEGIN
  SELECT COUNT(*)
  INTO val
  FROM t1;

DBMS_OUTPUT.PUT_LINE('TABLE COUNT BEFORE INSERT = '||val);

END;
/


-- PROCEDURES

--CREATE PROCEDURE print_clients_info AS
--BEGIN
--      SET NOCOUNT ON;
--
--      --DECLARE THE VARIABLES FOR HOLDING DATA.
--      DECLARE @Id INT
--             ,@title VARCHAR(16),
--             ,@fname VARCHAR(16),
--             ,@sname VARCHAR(16),
--
--
--      --DECLARE AND SET COUNTER.
--      DECLARE @Counter INT
--      SET @Counter = 1
--
--      --DECLARE THE CURSOR FOR A QUERY.
--      DECLARE print_clients CURSOR READ_ONLY
--      FOR
--      SELECT Id, title, fname, sname
--      FROM Client
--
--      --OPEN CURSOR.
--      OPEN print_clients
--
--      --FETCH THE RECORD INTO THE VARIABLES.
--      FETCH NEXT FROM print_clients INTO
--      @Id, @title, @fname, @sname
--
--      --LOOP UNTIL RECORDS ARE AVAILABLE.
--      WHILE @@FETCH_STATUS = 0
--      BEGIN
--             IF @Counter = 1
--             BEGIN
--                        PRINT 'Client ID' + CHAR(9) + 'Name'
--                        PRINT '------------------------------------'
--             END
--
--             --PRINT CURRENT RECORD.
--             PRINT CAST(@Id AS VARCHAR(16)) + CHAR(9) + @title + CHAR(1) + @fname + char(1) + @sname
--
--             --INCREMENT COUNTER.
--             SET @Counter = @Counter + 1
--
--             --FETCH THE NEXT RECORD INTO THE VARIABLES.
--             FETCH NEXT FROM print_clients INTO
--             @Id, @title, @fname, @sname
--      END
--
--      --CLOSE THE CURSOR.
--      CLOSE print_clients
--      DEALLOCATE print_clients
--END
--GO


DROP MATERIALIZED VIEW addTest;

CREATE MATERIALIZED VIEW LOG ON Test WITH PRIMARY KEY,ROWID(result)INCLUDING NEW VALUES;

CREATE MATERIALIZED VIEW addTest
CACHE
BUILD IMMEDIATE
REFRESH FAST ON COMMIT
ENABLE QUERY REWRITE
AS SELECT Test.result, COUNT(Test.result)
FROM Test
GROUP BY Test.result;

GRANT SELECT ON addTest TO xvodak05;

SELECT * from addTest;
INSERT INTO Test VALUES (NULL, TO_DATE('15.02.2019', 'dd.mm.yyyy'), '0', 2, 2);
COMMIT;
SELECT * from addTest;


-- YOUNG BLOOD TRIGGER

CREATE OR REPLACE TRIGGER young_blood
BEFORE INSERT
  on Donor
  FOR EACH ROW
DECLARE
donor_age number;
BEGIN
-- GET AGE
SELECT MONTHS_BETWEEN(TRUNC(sysdate), TO_DATE(:new.pId,'DD-MON-YYYY'))/12 INTO DONOR_AGE FROM DUAL;
    IF (DONOR_AGE > 21 ) THEN
    dbms_output.put_line('neco');
    ELSE
      RAISE_APPLICATION_ERROR(-20000,'TOO OLD.');
    END IF;
END;
/

CREATE OR REPLACE TRIGGER young_blood
BEFORE INSERT OR UPDATE OF pId on Donor
--BEFORE INSERT on Donor
  FOR EACH ROW
BEGIN
-- GET AGE
IF MONTHS_BETWEEN(current_date, :new.pId) > 21 * 12 THEN
      --RAISE_APPLICATION_ERROR(-20000,'TOO OLD 4 REDBank.', FALSE);
      RAISE cError;
      --dbms_output.put_line('2OLD');
    EXCEPTION
        WHEN cError THEN
        RAISE_APPLICATION_EXCEPTION(-20002, 'Train weight has exceeded max tow weight');
END IF;
END;
/


INSERT INTO Donor
VALUES (NULL, '1-MAY-1910','', 'P22awn', 'Sacrifice','M', 'Jupiter st.7', 'Cape Canaveral', 'jmc@nasawannabe.org', 'AB' , TO_DATE('09.03.2018', 'dd.mm.yyyy'));

INSERT INTO Donor
VALUES (NULL, '1-MAY-2010','', 'P33awn', 'Sacrifice','M', 'Jupiter st.7', 'Cape Canaveral', 'jmc@nasawannabe.org', 'AB' , TO_DATE('09.03.2018', 'dd.mm.yyyy'));



CREATE OR REPLACE TRIGGER check_donor_age
   AFTER INSERT
   ON Donor
   FOR EACH ROW
   DECLARE
   donor_age number;
BEGIN
   /* We don't want rows with a=2 inserted */
--   IF MONTHS_BETWEEN(current_date, :new.pId) > 21 * 12 THEN
--      OWA.cgi_var_name (OWA.cgi_var_name.COUNT + 1) := :NEW.ROWID;
SELECT MONTHS_BETWEEN(TRUNC(sysdate), TO_DATE(:new.pId,'DD-MON-YYYY'))/12 INTO DONOR_AGE FROM DUAL;
    IF (DONOR_AGE > 21 ) THEN
    dbms_output.put_line('DONOR IS TOO OLD, NOT ADDED!');
    OWA.cgi_var_name (OWA.cgi_var_name.COUNT + 1) := :NEW.ROWID;
   END IF;
END check_donor_age;
/

CREATE OR REPLACE TRIGGER delete_old_donors
   AFTER INSERT
   ON Donor
BEGIN
 /* We'll delete rows with a=2 again */
   FOR i IN 1 .. OWA.cgi_var_name.COUNT
   LOOP
      DELETE FROM Donor
            WHERE ROWID = OWA.cgi_var_name (i);
   END LOOP;
END delete_old_donors;
/
