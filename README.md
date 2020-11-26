LispInterpreter_in_C
=============
2020년 2학기 프로그래밍 언어론 이남규 교수님 프로젝트 과제  
이 교수님 프로젝트는 계속 반복될 가능성이 크므로 후배들은 유용하게 참고하세요  
물론 학점은 책임지지 않습니다.  
-CAU-

***

## 1.	LISP의 산술 연산 <span style="color:red">Finish!</span>   
* 사칙 연산(+, -, *, / )  
다음은 3+5의 계산과 (3+(5*6))의 계산이다.

  > -> (+ 3 5)  
  8  
  > -> (+ 3 (* 5 6))  
  33

***

## 2. LISP의 기본 함수
*	SETQ : 심볼에 값을 저장(Binding)  <span style="color:red">Finish!</span>   


  > -> (SETQ X 3);  X에 3을 저장한다  
  3


*	LIST : 원소들을 모아서 새로운 리스트 구조를 생성해 내는 함수이다.  `를 안붙이면 그 값을 계산해서 리스트에 넣는다.  
<span style="color:red">Finish!</span>   

  > -> (SETQ X 5);  X에 5를 저장  
  5  
  > -> X  
  5  
  > -> (LIST 'X X 'Y);  (X 5 Y)라는 리스트를 만듬


*	CAR : 리스트의 첫번째 원소를 가져온다.  
<span style="color:red">Finish!</span>   

  > -> (CAR '(X Y Z));   리스트 (X Y Z)의 첫 번째 원소인 X가 결과로 나옴.  
  X  
  > -> (CAR '((X) Y Z));  리스트 ((X) Y Z)의 첫 번째 원소인 (X)가 결과로 나옴.   
  (X)  


*	CDR : 리스트의 첫번째 원소를 제외한 나머지를 결과로 생성한다.  
<span style="color:red">Finish!</span>   
  > -> (CDR '(X Y Z));   리스트 (Y Z)가 결과로 나옴.  
  (Y Z)


*	CAR과 CDR을 혼합 사용할 수 있다
<span style="color:red">Finish!</span>   
  > -> (SETQ X ‘(1 2 3))  
  (1 2 3)
  
  <span style="color:red">Finish!</span>   
  > -> (CAR (CDR (CDR X)));  리스트 X에서 세번째 원소를 구한다   
  3  
  
  > -> (CADDR X);   위와 동일, 통합 명령으로 사용 가능  
  3


*	NTH : N번째 원소를 반환한다. 두번째 argument가 리스트가 아니면 에러  

  > -> (NTH 4 '(0 1 2 3 4 5 6));  4를 반환한다. (숫자를 0부터 셈에 주의)  
  4    
  > -> (NTH 3 '(A B));  NIL 반환.   
  NIL    
  > -> (NTH 3 'A);  Error   
  ERROR


•	CONS : 기존의 리스트에 새로운 원소를 추가하여 리스트를 만듬. 새로 추가되는 원소는 리스트의 첫번째 원소가 된다.
  > -> (CONS 'A '(B C D));  결과로 (A B C D)를 생성함.   
  (A B C D)


•	REVERSE : 주어진 리스트 안의 원소의 순서를 거꾸로 바꾼다.  
  > -> (REVERSE '(A B C D));  결과로 (D C B A)가 생성됨.  
  (D C B A)  


•	APPEND : 주어진 여러개의 리스트들을 하나의 리스트로 만든다.  
  > -> (APPEND '(A C) '(B D) '(E F));  결과로 (A C B D E F)가 생성됨.   
  (A C B D E F)  


•	LENGTH : 주어진 리스트 내의 원소 개수를 값으로 반환한다.  
  > -> (LENGTH '(A B C)); 주어진 리스트의 원소의 개수는 A와 B와 C. 총 3개이다.  
  3  
  > -> (LENGTH '((A B C)); 주어진 리스트의 원소의 개수는 (A B C) 하나이다.  
  1  


•	MEMBER : 주어진 리스트 내에 어떤 원소가 있는지 확인하는 함수이다.   
만약 찾고자 하는 원소가 주어진 리스트 내에 존재하면 그 원소부터 리스트 끝까지가 결과 값으로 반환된다.   
리스트 내에 찾고자 하는 원소가 없을 경우 NIL이 반환된다.  
  > -> (SETQ CLUB '(TOM HARRY JOHN DANIEL))  
  (TOM HARRY JOHN DANIEL)  
  > -> (MEMBER 'HARRY CLUB)    
  (HARRY JOHN DANIEL) ;  CLUB내의 원소 HARRY부터 그 리스트의 끝까지 반환됨  


•	ASSOC : 리스트를 원소로 같는 리스트에서 원소 리스트의 첫번째 원소를 데이터베이스에서의 KEY처럼 사용하여 원하는 리스트를 찾을 수 있는 함수이다.   
작은 데이터베이스 구축에 용이하게 쓸 수 있다.
  > -> (ASSOC 'TWO '((ONE 1)(TWO 2)(THREE 3)))    
  (TWO 2) ; 첫번째 원소가 TWO인 리스트를 찾아내서 리턴함  


•	REMOVE : 첫 번째 인자를 두 번째 인자로 받는 리스트에서 찾아 모두 제거하는 함수이다.
제거한 결과를 보여주기만 할 뿐, 그 결과값으로 리스트를 새로 저장하지는 않는다.   
  > -> (SETQ MYLIST '(A B C D E F))  
  (A B C D E F)    
  > -> (REMOVE 'D MYLIST)  
  (A B C E F) ;  D가 삭제되었다.  
  > -> (SETQ MYLIST '(A D B C D E D F))  
  > -> (REMOVE D MYLIST)  
  (A B C E F) ;  D가 중복되어 여러개가 있어도 모두 삭제한다.  


•	SUBST : 세 번째 인자에서 두 번째 인자를 찾아 첫 번째 인자로 대치한다.   
  > -> (SUBST 'GOOD 'BAD '(I AM BAD))   
  (I AM GOOD) ;  BAD가 GOOD으로 대치되었다.  

***

## 3. LISP의 Predicate 함수 
T(true)나 NIL(false)를 반환하는 함수이다  
LISP에서 Predicate가 NIL을 반환하면 거짓(false)로 인정하고 NIL이 아닌 다른 어떤 값을 반환하면 참(true)으로 인정한다.   
함수 이름 뒤에 Predicate의 P를 붙인 것들이 많다.  


  > (ATOM  X) ;  X가 ATOM(심볼)일 때만 참(true)를 반환함.  
  
  > (NULL X) ;  X가 NIL일 때만 참(true)을 반환함.  
  
  > (NUMBERP X) ;  X가 숫자일 때만 참(true)을 반환함.  
  
  > (ZEROP X) ;  X가 0일 때만 참(true)을 반환함. X가 숫자가 아니면 ERROR 발생.   
  
  > (MINUSP X) ; X가 음수일 때만 참(true)을 반환함. X가 숫자가 아니면 ERROR 발생.  
  
  > (EQUAL X Y) ;  X와 Y가 같으면 참(true)을 반환함.  
  
  > (< X Y) ;  X < Y 이면 참(true)을 반환함.  
  
  > (>= X Y) ;  X >= Y 이면 참(true)을 반환함.  
  
  > (STRINGP X) ;  X가 STRING일 때만 참(true)을 반환함.  
  > -> (STRINGP "A")   
  T
  > -> (SETQ A "HI THERE")  
  > (STRINGP A)  
  T  
  > (STRINGP #\A) ;  문자  
  NIL  
  > (STRINGP '(A B C)) ;  리스트  
  NIL  
  > (STRINGP 1.2) ;  숫자  
  NIL  
  > (STRINGP 'A) ;  심볼  
  NIL  
  > (STRINGP #(0 1 2)) ;  배열  
  NIL  
  > (STRINGP NIL)  
  NIL  

***

## 4. LISP의 조건문
•	IF
  > -> (IF (> X 3) (PRINT X)) ;  X값이 3보다 크면 X값을 프린트 아니면 NIL.  
  > -> (IF (> X 3) (PRINT X) (+ X 5) ; X값이 3보다 크면 X값이 프린트되고; 그렇지 않으면 X에 5를 더하게 됨.  
  
 
•	COND
 (COND (<제 1 조건문> <수행문1>)  
         (<제 2 조건문> <수행문2>)  
         (<제 3 조건문> <수행문3>)  

  > -> (COND ((> X 0) (+ X 1))  ; X 가 0보다 크면 X 값에 1을 더함  
  ((= X 0) (+ X 2))  ; X 가 0이면 X 값에 2을 더함  
  ((< X 0) (+ X 3)))  ; X 가 0보다 작으면 X 값에 3을 더함  
