#include <stdio.h>

typedef struct {
    int t;
    int m;

}PhanSo;
void nhapPhanSo(PhanSo *a,int n){
    for(int i=0;i<n; i++){
        printf("Tu so cua phan so %d: ",i);
        scanf("%d",&a[i].t);
        printf("Mau so cua phan so %d",i);
        scanf("%d",&a[i].m);
    }
}
int timPhanSo(PhanSo *a,PhanSo b,int n){
    for(int i=0;i< n ;i++){
        if(a[i].t == b.t && a[i].m == b.m){
            return i;
        }
    }
    return -1;
}
PhanSo* sapXepPhanSo(PhanSo *a, int n){
    PhanSo tmp;
    for(int i=0;i<n-1;i++)
		for(int j=i+1;j<n;j++){
            if(a[i].t/a[i].m < a[j].t/a[j].m)
			tmp =a[i];
			a[i]=a[j];
			a[j]=tmp;    
        }
    return a;
}
int binarySearch(PhanSo *a, int l, int r, PhanSo x){
     if (r >= l) {
    int mid = l + (r - l) / 2;
    if (a[mid].t/a[mid].m == x.t/x.m)
      return mid;
    if (a[mid].t/a[mid].m > x.t/x.m)
      return binarySearch(a, l, mid - 1, x);
    return binarySearch(a, mid + 1, r, x);
  }
  return -1;
}
int main(){
    PhanSo a[100];

}