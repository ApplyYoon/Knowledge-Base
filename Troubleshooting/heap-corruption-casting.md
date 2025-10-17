# Heap Corruption caused by Wrong Struct Casting

## 문제 상황
Red-Black Tree를 구현하던 중 프로그램이 비정상 종료되었는데,  
출력 로그는 아래와 같았다.  
```Fatal glibc error: malloc.c:2599 (sysmalloc): assertion failed:  
(old_top == initial_top (av) && old_size == 0) || ...
```  

## 원인 분석
문제의 원인은 **구조체 타입 캐스팅 오류**였다.

```
// 문제 코드 (C)
rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

// 해결 코드
node_t *p = (node_t *)calloc(1, sizeof(node_t));
```
  
위 코드에서 할당한 변수는 ‘노드’처럼 사용하기 위한 변수였는데,  
‘트리’ 구조체 크기(sizeof(rbtree))로 메모리를 잡는 바람에  
실제 필드 접근 시 할당된 영역을 초과하여 힙 메모리가 손상되었다.    

이후 다른 calloc()이나 free() 호출 시,  
이미 손상된 힙 메모리를 검사하던 glibc 내부의 힙 검증 로직(sysmalloc)에서  
assertion failure가 발생했다.  

### 손상된 힙 메모리를 검사할 때 glibc 내부의 힙 검증에서 오류가 뜨는 원인  
구체적으로 왜, 어떻게 오류가 뜨는지가 궁금해 추가로 찾아봤다. 

#### 1. glibc의 힙 관리 기본 구조
glibc의 malloc()은 단순히 메모리 덩어리 하나 할당이 아니라,  
힙 공간 전체를 '청크(chunk)'라는 단위로 관리한다.  

또한 각각의 할당 블록에는 메타데이터(header)가 붙는데,  
이 안에는 다음 정보가 저장된다.  
```[ size | prev_size | flgas | user_data ... ]```  

이 메타데이터를 통해 glibc는,  
- 어떤 블록이 사용 중인지 (inuse flag)  
- 다음 블록의 시작 위치  
- 병합(coalesce) 가능한지 여부  
를 추적한다.  
  
#### 2. 힙 손상이란?
"사용자 코드가 의도치 않게 메타데이터를 덮어쓴 상태"이다.  

예를 들어,  
sizeof(node_t)보다 작은 크기로 calloc()을 했는데  
그 포인트를 node_t처럼 써서 p->left, p->right 등에 접근하면  
그 뒤쪽의 힙 메타데이터(header 영역)이 덮어씌어진다.  

```
[ Header | User Data ]
          ↑
          여기까지만 써야 하는데 초과 접근 → Header 손상
```

#### 3. glibc 검증 로직이 오류를 내는 시점
glibc는 새로운 malloc/calloc/free 호출 시점마다  
이전 블록의 메타데이터를 검사한다.  

특히 필자가 오류가 난, 내부 함수 sysmalloc() (malloc.c:2599) 부근에서는
아래와 같은 조건을 체크한다고 한다.
```
assert(
    (old_top == initial_top (av) && old_size == 0)
    || ((unsigned long) (old_size) >= MINSIZE
        && prev_inuse (old_top)
        && ((unsigned long) old_end & (pagesize - 1)) == 0)
);
```
정확하게 해석하긴 어렵지만, 서칭해보니 "현재 힙의 top chunck 상태가 유효한가?"를 확인하는 코드라고 한다.  

그런데,  
- 이전에 힙 메타데이터가 이미 덮어써져 있으면  
- old_size나 prev_inuse(old_top) 값이 엉뚱한 값이 되어  
- 이 assert 조건이 거짓(false) 이 되어  
바로 프로그램이 abort()된다고 한다.

결국 이 에러는 malloc을 잘못 선언해서 바로 에러가 터진게 아니라,  
힙 손상으로 인한 후폭풍이다 :(  
