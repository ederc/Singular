// emacs edit mode for this file is -*- C++ -*-
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: convert data between Singular and Flint
*/



#include "misc/auxiliary.h"
#include "flintconv.h"

#ifdef HAVE_FLINT
#if __FLINT_RELEASE >= 20500
#include "coeffs/coeffs.h"
#include "coeffs/longrat.h"
#include "polys/monomials/p_polys.h"

#include "polys/sbuckets.h"
#include "polys/clapconv.h"

#include "simpleideals.h"


int convFlintISingI (fmpz_t f)
{
  int res;
  res = fmpz_get_si(f);
  return res;
}

void convSingIFlintI(fmpz_t f, int p)
{
  fmpz_init(f);
  fmpz_set_si(f,p);
  return;
}

void convFlintNSingN (mpz_t z, fmpz_t f)
{
  mpz_init(z);
  fmpz_get_mpz(z,f);
}

number convFlintNSingN (fmpz_t f)
{
  mpz_t z;
  mpz_init(z);
  fmpz_get_mpz(z,f);
  number n;
  nlMPZ(z,n,NULL);
  return n;
}
number convFlintNSingN (fmpq_t f)
{
  number z=ALLOC_RNUMBER();
#if defined(LDEBUG)
  z->debug=123456;
#endif
  mpz_init(z->z);
  mpz_init(z->n);
  fmpq_get_mpz_frac(z->z,z->n,f);
  nlNormalize(z,NULL);
  return z;
}

void convSingNFlintN(fmpz_t f, mpz_t n)
{
  fmpz_init(f);
  fmpz_set_mpz(f,n);
}

void convSingNFlintN(fmpz_t f, number n)
{
  fmpz_init(f);
  fmpz_set_mpz(f,(mpz_ptr)n);
}

void convSingNFlintN(fmpq_t f, number n)
{
  fmpq_init(f);
  if (SR_HDL(n)&SR_INT)
    fmpq_set_si(f,SR_TO_INT(n),1);
  else if (n->s<3)
  {
    fmpz_set_mpz(fmpq_numref(f), n->z);
    fmpz_set_mpz(fmpq_denref(f), n->n);
  }
  else
  {
    mpz_t one;
    mpz_init_set_si(one,1);
    fmpz_set_mpz(fmpq_numref(f), n->z);
    fmpz_set_mpz(fmpq_denref(f), one);
    mpz_clear(one);
  }
}

void convSingPFlintP(fmpq_poly_t res, poly p, const ring r)
{
  int d=p_GetExp(p,1,r);
  fmpq_poly_init2(res,d+1);
  _fmpq_poly_set_length (res, d + 1);
  while(p!=NULL)
  {
    number n=pGetCoeff(p);
    fmpq_t c;
    convSingNFlintN(c,n);
    fmpq_poly_set_coeff_fmpq(res,p_GetExp(p,1,r),c);
    fmpq_clear(c);
    pIter(p);
  }
}

poly convFlintPSingP(fmpq_poly_t f, const ring r)
{
  int d=fmpq_poly_length(f);
  poly p=NULL;
  for(int i=0; i<=d; i++)
  {
    fmpq_t c;
    fmpq_poly_get_coeff_fmpq(c,f,i);
    number n=convFlintNSingN(c);
    poly pp=p_Init(r);
    pSetCoeff0(pp,n);
    p_SetExp(pp,1,i,r);
    p_Setm(pp,r);
    p=p_Add_q(p,pp,r);
  }
  return p;
}

bigintmat* singflint_LLL(bigintmat*  m, bigintmat* T)
{
  int r=m->rows();
  int c=m->cols();
  bigintmat* res=new bigintmat(r,c,m->basecoeffs());
  fmpz_mat_t M, Transf;
  fmpz_mat_init(M, r, c);
  if(T != NULL)
  {
    fmpz_mat_init(Transf, T->rows(), T->rows());
  }
  fmpz_t dummy;
  mpz_t n;
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      n_MPZ(n, BIMATELEM(*m, i, j),m->basecoeffs());
      convSingNFlintN(dummy,n);
      mpz_clear(n);
      fmpz_set(fmpz_mat_entry(M, i-1, j-1), dummy);
      fmpz_clear(dummy);
    }
  }
  if(T != NULL)
  {
    for(i=T->rows();i>0;i--)
    {
      for(j=T->rows();j>0;j--)
      {
        n_MPZ(n, BIMATELEM(*T, i, j),T->basecoeffs());
        convSingNFlintN(dummy,n);
        mpz_clear(n);
        fmpz_set(fmpz_mat_entry(Transf, i-1, j-1), dummy);
        fmpz_clear(dummy);
      }
    }
  }
  fmpz_lll_t fl;
  fmpz_lll_context_init_default(fl);
  if(T != NULL)
    fmpz_lll(M, Transf, fl);
  else
    fmpz_lll(M, NULL, fl);
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      convFlintNSingN(n, fmpz_mat_entry(M, i-1, j-1));
      n_Delete(&(BIMATELEM(*res,i,j)),res->basecoeffs());
      BIMATELEM(*res,i,j)=n_InitMPZ(n,res->basecoeffs());
      mpz_clear(n);
    }
  }
  if(T != NULL)
  {
    for(i=T->rows();i>0;i--)
    {
      for(j=T->cols();j>0;j--)
      {
        convFlintNSingN(n, fmpz_mat_entry(Transf, i-1, j-1));
        n_Delete(&(BIMATELEM(*T,i,j)),T->basecoeffs());
        BIMATELEM(*T,i,j)=n_InitMPZ(n,T->basecoeffs());
        mpz_clear(n);
      }
    }
  }
  return res;
}

intvec* singflint_LLL(intvec*  m, intvec* T)
{
  int r=m->rows();
  int c=m->cols();
  intvec* res = new intvec(r,c,(int)0);
  fmpz_mat_t M,Transf;
  fmpz_mat_init(M, r, c);
  if(T != NULL)
    fmpz_mat_init(Transf, r, r);
  fmpz_t dummy;
  int i,j;
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      convSingIFlintI(dummy,IMATELEM(*m,i,j));
      fmpz_set(fmpz_mat_entry(M, i-1, j-1), dummy);
      fmpz_clear(dummy);
    }
  }
  if(T != NULL)
  {
    for(i=T->rows();i>0;i--)
    {
      for(j=T->rows();j>0;j--)
      {
        convSingIFlintI(dummy,IMATELEM(*T,i,j));
        fmpz_set(fmpz_mat_entry(Transf, i-1, j-1), dummy);
        fmpz_clear(dummy);
      }
    }
  }
  fmpz_lll_t fl;
  fmpz_lll_context_init_default(fl);
  if(T != NULL)
    fmpz_lll(M, Transf, fl);
  else
    fmpz_lll(M, NULL, fl);
  for(i=r;i>0;i--)
  {
    for(j=c;j>0;j--)
    {
      IMATELEM(*res,i,j)=convFlintISingI(fmpz_mat_entry(M, i-1, j-1));
    }
  }
  if(T != NULL)
  {
    for(i=Transf->r;i>0;i--)
    {
      for(j=Transf->r;j>0;j--)
      {
        IMATELEM(*T,i,j)=convFlintISingI(fmpz_mat_entry(Transf, i-1, j-1));
      }
    }
  }
  return res;
}
#endif
#endif
