// QuinMcCluskey.cpp: implementation of the CQuinMcCluskey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogObvody.h"
#include "QuinMcCluskey.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuinMcCluskey::CQuinMcCluskey(CWnd *parent)
{
	m_BoolFunctionValues=NULL;
	m_VariablesCount=0;
	m_ValuesCount=0;
	m_pDoc=NULL;
	m_pStatusDlg=NULL;
	m_pParent=parent;
	m_Status=STATUS_NONE;
}

CQuinMcCluskey::~CQuinMcCluskey()
{
	SAFE_DELETE(m_BoolFunctionValues);
}

int CQuinMcCluskey::SetFunction(BYTE *values,DWORD variablesCount)
{
	SAFE_DELETE(m_BoolFunctionValues);
	m_VariablesCount=variablesCount;
	m_ValuesCount=pow2(variablesCount);
	m_BoolFunctionValues=new BYTE[m_ValuesCount];
	if(m_BoolFunctionValues==NULL)return -1;
	DWORD k;
	for(k=0;k<m_ValuesCount;k++)m_BoolFunctionValues[k]=values[k];
	return 0;
}

void CQuinMcCluskey::MessagePump(void)
{
	MSG msg;

	if(::PeekMessage(&msg, NULL, 0, 0,PM_REMOVE)){
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void CQuinMcCluskey::MinimalizeFunction(SImplikant *finalImplikants,DWORD *count)
{
	if(m_pStatusDlg==NULL)return;
	MessagePump();
	//if(m_pDoc==NULL)return;
	finalImplikants[0].flag=BIT_UNDEFINED;
	if(m_BoolFunctionValues==NULL){		
		*count=0;
		return;
	}
	SImplikant *implikants;
	SImplikant *implikants2; //2. tabulka
	SImplikant *prosteImplikanty; //potencionalne finalne implikanty

	DWORD k,i;
	//rozdelenie jednotkovych bodov do skupin podla poctu jednotiek v bin. tvare vstupu
	DWORD countOne=0;
	for(k=0;k<m_ValuesCount;k++)if(m_BoolFunctionValues[k]==1 || m_BoolFunctionValues[k]==2)countOne++;
	//osetrenie specialnych pripadov (jednotkova fcia resp. nulova fcia)
	if(countOne==m_ValuesCount){
		finalImplikants[0].flag=BIT_ONE;
		*count=0;
		m_Status=STATUS_FINISH;
		return;
	}
	if(countOne==0){
		finalImplikants[0].flag=BIT_ZERO;
		*count=0;
		m_Status=STATUS_FINISH;
		return;
	}

	DWORD maxArraySize=countOne,maxArraySize2=countOne;
	implikants=new SImplikant[maxArraySize];
	implikants2=new SImplikant[maxArraySize];
	prosteImplikanty=new SImplikant[maxArraySize2];
	DWORD pocetProstychImplikantov=0;
	countOne=0;
	for(k=0;k<m_ValuesCount;k++)if(m_BoolFunctionValues[k]==1 || m_BoolFunctionValues[k]==2){
		implikants[countOne].count=0;
		implikants[countOne].indexList=new SIndexList;
		implikants[countOne].indexList->index[0]=k;
		implikants[countOne].indexList->indexCount=1;
		implikants[countOne].indexList->next=NULL;

		implikants[countOne].flag=FLAG_UNUSED;
		for(i=0;i<m_VariablesCount;i++){
			implikants[countOne].binValues[i]=getBitValue(k,i);
			if(implikants[countOne].binValues[i]==1)implikants[countOne].count++;
		}		
		countOne++;
	}
	//usporiada pole podla poctu jednotiek do skupin a podla indexu v ramci skupiny
	Sort(implikants,countOne);

	//zapis do dokumentu -> rozdelenie do skupin podla poctu jedniciek
	//v jednickoch a neurcenych bodoch
	if(m_pDoc!=NULL){
		m_pDoc->m_GroupValuesCount=m_VariablesCount+1;
		m_pDoc->m_GroupValues=new SGroupValues[m_pDoc->m_GroupValuesCount];
		for(i=0;i<m_pDoc->m_GroupValuesCount;i++){
			m_pDoc->m_GroupValues[i].de=NULL;
			m_pDoc->m_GroupValues[i].countDe=0;
			for(k=0;k<countOne;k++)if(implikants[k].count==i)m_pDoc->m_GroupValues[i].countDe++;
			if(m_pDoc->m_GroupValues[i].countDe>0){
				m_pDoc->m_GroupValues[i].de=new DWORD[m_pDoc->m_GroupValues[i].countDe];
				DWORD tmpIndex=0;
				for(k=0;k<countOne;k++)if(implikants[k].count==i)m_pDoc->m_GroupValues[i].de[tmpIndex++]=implikants[k].indexList->index[0];
			}
		}
	}

	//pospajanie hodnot
	*count=0;
	DWORD countImplikants,countImplikants2;
	countImplikants=countOne;
	countImplikants2=0;

	//zapis do dokumentu -> medzivylsedky
	PSInterImplikant aktInterImplikants=NULL;

	BOOL finish;

	char strDebug[256];
	int cyclusCounter=0;
	do{
		finish=TRUE;
		for(k=0;k<countImplikants-1;k++){
			for(i=k+1;i<countImplikants;i++){
				if(CanMerge(implikants[k],implikants[i])){
					if(countImplikants2+1>=maxArraySize){
						//je potrebne resiznut velkost pola
						//MessageBox(NULL,"break 1","DEBUG",MB_OK);
						ResizeMemory((BYTE**)&implikants,sizeof(SImplikant),maxArraySize,maxArraySize+MEMORY_SIZE);
						ResizeMemory((BYTE**)&implikants2,sizeof(SImplikant),maxArraySize,maxArraySize+MEMORY_SIZE);
						maxArraySize+=MEMORY_SIZE;
						//MessageBox(NULL,"break 2","DEBUG",MB_OK);
					}
					MergeImplikants(&implikants2[countImplikants2],implikants[k],implikants[i]);
					implikants[k].flag=FLAG_USED;
					implikants[i].flag=FLAG_USED;
					countImplikants2++;
					finish=FALSE;
				}
				MessagePump();
				
				if(m_Status==STATUS_STOP){
					m_Status=STATUS_STOPPED;
					*count=0;
					SAFE_DELETE(implikants);
					SAFE_DELETE(implikants2);
					SAFE_DELETE(prosteImplikanty);
					return;
				}
			}
			MessagePump();
			if((k % 10)==0){
				sprintf(strDebug,"spracuvavam implikant: %d",k);
				m_pStatusDlg->GetDlgItem(IDC_STATIC_TEXT4)->SetWindowText(strDebug);
				sprintf(strDebug,"nasledujuci pocet implikantov: %d",countImplikants2);
				m_pStatusDlg->GetDlgItem(IDC_STATIC_TEXT5)->SetWindowText(strDebug);
			}
			
			if(m_Status==STATUS_STOP){
				m_Status=STATUS_STOPPED;
				*count=0;
				SAFE_DELETE(implikants);
				SAFE_DELETE(implikants2);
				SAFE_DELETE(prosteImplikanty);
				return;
			}
		}

		if(m_pDoc!=NULL){
			//zapis medzivysledku po spojeni
			if(countImplikants>0){
				if(aktInterImplikants==NULL){
					//zapis prveho medzivysledku
					m_pDoc->m_InterImplikants=new SInterImplikant;
					aktInterImplikants=m_pDoc->m_InterImplikants;
				}else{
					//zapis dalsich medzivysledkov
					aktInterImplikants->next=new SInterImplikant;
					aktInterImplikants=aktInterImplikants->next;
				}
				aktInterImplikants->next=NULL;
				aktInterImplikants->countImplikants=countImplikants;
				aktInterImplikants->implikants=new SImplikant[countImplikants];
				for(k=0;k<countImplikants;k++)
					CopyImplikant(&aktInterImplikants->implikants[k],&implikants[k]);
			}
		}

		//prehodim tabulky
		//nepouzite implikanty su proste implikanty, tj. su vystupom
		for(k=0;k<countImplikants;k++){
			if(implikants[k].flag==FLAG_UNUSED){
				//je to prosty implikant
				if(pocetProstychImplikantov+1>=maxArraySize2){
					ResizeMemory((BYTE**)&prosteImplikanty,sizeof(SImplikant),maxArraySize2,maxArraySize2+MEMORY_SIZE);
					maxArraySize2+=MEMORY_SIZE;
				}
				prosteImplikanty[pocetProstychImplikantov++]=implikants[k];
			}else{
				//bol uz pouzity -> mozem ho zahodit
				KillImplikant(&implikants[k]);
			}
		}

		//pokracovanie metody
		if(!finish){
			for(k=0;k<countImplikants2;k++)implikants[k]=implikants2[k];
			countImplikants=countImplikants2;
			countImplikants2=0;
			
			m_pStatusDlg->GetDlgItem(IDC_STATIC_TEXT4)->SetWindowText("Odstranujem duplicitne implikanty");
			//odstrania sa duplicitne implikanty
			int kk,ii;
			for(kk=0;kk<(int)countImplikants-1;kk++){
				if((kk % 10)==0){
					sprintf(strDebug,"testujem implikant: %d",kk);
					m_pStatusDlg->GetDlgItem(IDC_STATIC_TEXT5)->SetWindowText(strDebug);
				}
				for(ii=kk+1;ii<(int)countImplikants;ii++){
					if(isIdentical(&implikants[kk],&implikants[ii])){
						implikants[ii--]=implikants[--countImplikants];
					}
				}
				if((kk % 10)==0){
					sprintf(strDebug,"aktualny pocet implikantov: %d",countImplikants);
					m_pStatusDlg->GetDlgItem(IDC_STATIC_TEXT6)->SetWindowText(strDebug);
				}
			}			
			m_pStatusDlg->GetDlgItem(IDC_STATIC_TEXT6)->SetWindowText("");
		}
		
		cyclusCounter++;
		sprintf(strDebug,"cyklus cislo. %d",cyclusCounter);
		m_pStatusDlg->GetDlgItem(IDC_STATIC_TEXT2)->SetWindowText(strDebug);
		sprintf(strDebug,"pocet implikantov=%d",countImplikants);
		m_pStatusDlg->GetDlgItem(IDC_STATIC_TEXT3)->SetWindowText(strDebug);
		
		MessagePump();
		if(m_Status==STATUS_STOP){
			m_Status=STATUS_STOPPED;
			*count=0;
			SAFE_DELETE(implikants);
			SAFE_DELETE(implikants2);
			SAFE_DELETE(prosteImplikanty);
			return;
		}
	}while(!finish);

	//z prostych implikantov vyberiem tie, ktore su minimalne a tak, aby bola pokryta
	//cela funkcia
	UrcenieIDNF(prosteImplikanty,&pocetProstychImplikantov);

	if(m_Status!=STATUS_STOPPED){
		for(k=0;k<pocetProstychImplikantov;k++)finalImplikants[k]=prosteImplikanty[k];
		*count=pocetProstychImplikantov;
	}

	SAFE_DELETE(implikants);
	SAFE_DELETE(implikants2);
	SAFE_DELETE(prosteImplikanty);
	m_Status=STATUS_FINISH;
}

void CQuinMcCluskey::Sort(SImplikant *arr,DWORD arrLength)
{
	DWORD k,i,minIndex;
	for(k=0;k<arrLength-1;k++){
		minIndex=k;
		for(i=k+1;i<arrLength;i++){
			if(arr[i].count==arr[minIndex].count && arr[i].indexList->index[0]<arr[minIndex].indexList->index[0])minIndex=i;
			if(arr[i].count<arr[minIndex].count)minIndex=i;
		}
		SImplikant tmpBitCount=arr[k];
		arr[k]=arr[minIndex];
		arr[minIndex]=tmpBitCount;
	}
}

BOOL CQuinMcCluskey::CanMerge(SImplikant impl1,SImplikant impl2)
{
	//implikanty moze byt spojene, iba ak sa lisia v jednom bite (zmena 0-1)
	DWORD k,changesCount=0;
	for(k=0;k<m_VariablesCount;k++){
		if(impl1.binValues[k]!=impl2.binValues[k])changesCount++;
	}

	return (changesCount==1);
}

void CQuinMcCluskey::MergeImplikants(SImplikant *outimpl,SImplikant impl1,SImplikant impl2)
{
	DWORD k;
	for(k=0;k<m_VariablesCount;k++){
		if(impl1.binValues[k]!=impl2.binValues[k]){
			outimpl->binValues[k]=BIT_UNDEFINED;
		}else outimpl->binValues[k]=impl1.binValues[k];
	}
	outimpl->flag=FLAG_UNUSED;
	outimpl->indexList=NULL;
	outimpl->indexList=new SIndexList;
	outimpl->indexList->indexCount=0;
	outimpl->indexList->next=NULL;
	PSIndexList aktOutIndex=outimpl->indexList,aktIndex=impl1.indexList;
	while(aktIndex!=NULL){
		//ak vojdu vsetky indexy do danej polozky -> netreba vytvarat dalsie miesto
		for(k=0;k<aktIndex->indexCount;k++){
			if(aktOutIndex->indexCount+1<MAX_INDEX_COUNT){
				aktOutIndex->index[aktOutIndex->indexCount++]=aktIndex->index[k];
			}else{
				aktOutIndex->next=new SIndexList;
				aktOutIndex=aktOutIndex->next;
				aktOutIndex->next=NULL;
				aktOutIndex->indexCount=0;
				aktOutIndex->index[aktOutIndex->indexCount++]=aktIndex->index[k];
			}
		}
		aktIndex=aktIndex->next;
	}
	aktIndex=impl2.indexList;
	while(aktIndex!=NULL){
		//ak vojdu vsetky indexy do danej polozky -> netreba vytvarat dalsie miesto
		for(k=0;k<aktIndex->indexCount;k++){
			if(aktOutIndex->indexCount+1<MAX_INDEX_COUNT){
				aktOutIndex->index[aktOutIndex->indexCount++]=aktIndex->index[k];
			}else{
				aktOutIndex->next=new SIndexList;
				aktOutIndex=aktOutIndex->next;
				aktOutIndex->next=NULL;
				aktOutIndex->indexCount=0;
				aktOutIndex->index[aktOutIndex->indexCount++]=aktIndex->index[k];
			}
		}
		aktIndex=aktIndex->next;
	}
}

BOOL CQuinMcCluskey::isIdentical(SImplikant *impl1,SImplikant *impl2)
{
	DWORD k;
	for(k=0;k<m_VariablesCount;k++)if(impl1->binValues[k]!=impl2->binValues[k])return FALSE;
	return TRUE;
}

void CQuinMcCluskey::UrcenieIDNF(SImplikant *prosteImplikanty,DWORD *pocetProstychImplikantov)
{
	//odstrania sa duplicitne implikanty
	DWORD k,i,j,count=*pocetProstychImplikantov;
	for(k=0;k<count-1;k++){
		for(i=k+1;i<count;i++){
			if(isIdentical(&prosteImplikanty[k],&prosteImplikanty[i])){
				prosteImplikanty[i--]=prosteImplikanty[--count];
			}
		}
	}
	*pocetProstychImplikantov=count;

	//v SImplikant::count bude rad implikantu (tj. pocet premennych)
	for(k=0;k<count;k++){
		prosteImplikanty[k].count=0;
		for(i=0;i<m_VariablesCount;i++)if(prosteImplikanty[k].binValues[i]!=BIT_UNDEFINED)prosteImplikanty[k].count++;
		prosteImplikanty[k].flag=0;
	}
	//vytvorenie MPI -> mriezka prostych implikantov
	DWORD rowsCount,columnsCount;
	rowsCount=count;
	columnsCount=0;
	for(k=0;k<m_ValuesCount;k++)if(m_BoolFunctionValues[k]==1)columnsCount++;
	BYTE *tableMPI=new BYTE[rowsCount*columnsCount];
	if(tableMPI==NULL)return;
	for(k=0;k<rowsCount*columnsCount;k++)tableMPI[k]=0;
	DWORD *FunctionPoint=new DWORD[columnsCount];
	if(FunctionPoint==NULL){
		SAFE_DELETE(tableMPI);
		return;
	}
	columnsCount=0;
	for(k=0;k<m_ValuesCount;k++)if(m_BoolFunctionValues[k]==1)FunctionPoint[columnsCount++]=k;

	//oznacenie stlpcov a riadkov
	BYTE *rowsSelect,*columnsSelect;
	rowsSelect=new BYTE[rowsCount];
	columnsSelect=new BYTE[columnsCount];
	for(k=0;k<rowsCount;k++)rowsSelect[k]=FALSE;
	for(k=0;k<columnsCount;k++)columnsSelect[k]=FALSE;

	//vyplnenie MPI
	for(k=0;k<rowsCount;k++){
		PSIndexList indexList=prosteImplikanty[k].indexList;
		while(indexList!=NULL){
			for(i=0;i<indexList->indexCount;i++){
				//test ci to je jednotkovy bod fcie (moze byt aj neurceny)
				if(m_BoolFunctionValues[indexList->index[i]]==1){
					//zapisat 'krizik' do tabulky MPI
					for(j=0;j<columnsCount;j++)if(FunctionPoint[j]==indexList->index[i])
						tableMPI[j+k*columnsCount]=1;
				}
			}
			indexList=indexList->next;
		}		
	}

	DWORD crossCount,rowIndex;
	BOOL dnfFinish;
	do{
		MessagePump();
		if(m_Status==STATUS_STOP){
			m_Status=STATUS_STOPPED;
			SAFE_DELETE(tableMPI);
			SAFE_DELETE(rowsSelect);
			SAFE_DELETE(columnsSelect);
			SAFE_DELETE(FunctionPoint);
			return;
		}
		//KROK 1
		for(k=0;k<columnsCount;k++)if((columnsSelect[k] & ITEM_SELECT)==0){
			//ak v danom stlpci je iba jeden 'krizik' potom riadok, v kt. je tento 'krizik' udava
			//podstatny implikant
			crossCount=0;
			for(i=0;i<rowsCount;i++)if(tableMPI[k+i*columnsCount]==1 && (rowsSelect[i] & ITEM_SELECT)==0){
				crossCount++;
				rowIndex=i;
			}
			if(crossCount==1){
				rowsSelect[rowIndex]|= ITEM_SELECT | ITEM_IS_DNF;
				//vyskrtavanie stlpcov, v ktorych ma implikant 'krizik'
				for(i=0;i<columnsCount;i++)if(tableMPI[i+rowIndex*columnsCount]==1)columnsSelect[i]|=ITEM_SELECT;
			}
		}

		dnfFinish=TRUE;
		for(k=0;k<columnsCount;k++)if((columnsSelect[k] & ITEM_SELECT)==0)dnfFinish=FALSE;

		//aby sa nezacyklilo
		//moze nastat pripad (chybovy stav) ze niesu vyskrtane vsetky stlpce, ale riadky su
		//vyskrtane vsetky
		if(!dnfFinish){
			dnfFinish=TRUE;
			for(k=0;k<rowsCount;k++)if((rowsSelect[k] & ITEM_SELECT)==0)dnfFinish=FALSE;
		}

		if(!dnfFinish){
			//KROK 2
			//vykrtavanie implikantov (riadkov), ktore su pokryte inymi implikantami, ktore su 
			//mensieho radu ako dany implikant
			for(k=0;k<rowsCount;k++)if((rowsSelect[k] & ITEM_SELECT)==0){
				//testujem, ci tento riadok je potrebne vykrtnut
				for(i=0;i<rowsCount;i++)if(i!=k && prosteImplikanty[k].count>=prosteImplikanty[i].count && (rowsSelect[i] & ITEM_SELECT)==0){
					//v i-tom riadku je implikant mensieho radu -> ak pokryva k-ty implikant,
					//potom sa k-ty riadok skrta
					BOOL pokryva=TRUE;
					for(j=0;j<columnsCount;j++)if((columnsSelect[j] & ITEM_SELECT)==0 && tableMPI[j+k*columnsCount]==1 && tableMPI[j+i*columnsCount]==0){
						pokryva=FALSE;
					}
					if(pokryva)rowsSelect[k]|=ITEM_SELECT;
				}
			}

			//vyber stlpca s min. poctom krizikov (neoznaceneho stlpca)
			DWORD columnIndex=0;
			DWORD minCrossCount=rowsCount+1;
			for(k=0;k<columnsCount;k++)if((columnsSelect[k] & ITEM_SELECT)==0){
				crossCount=0;
				for(i=0;i<rowsCount;i++)if(tableMPI[k+i*columnsCount]==1 && (rowsSelect[i] & ITEM_SELECT)==0)crossCount++;
				if(crossCount<minCrossCount){
					minCrossCount=crossCount;
					columnIndex=k;
				}
			}
			//ak minCrossCount==1 -> potom to je podstatny implikant, ten sa oznaci v KROKU 1
			if(minCrossCount>=2){
				//z daneho stlpva sa vyberie prvy neoznaceny krizik a riadok s tymto krizikom
				//sa oznaci -> tj. implikant bude v dnf
				for(k=0;k<rowsCount;k++)if((rowsSelect[k] & ITEM_SELECT)==0 && tableMPI[columnIndex+k*columnsCount]==1){
					//tento krizik za vyberie
					rowsSelect[k]|= ITEM_SELECT | ITEM_IS_DNF;
					rowIndex=k;
					for(i=0;i<columnsCount;i++)if(tableMPI[i+rowIndex*columnsCount]==1)columnsSelect[i]|=ITEM_SELECT;
					break;
				}
			}
		}
	}while(!dnfFinish);

	if(m_pDoc!=NULL){
		//zapis do dokumentu -> vyplnenie tabulky pre zobrazenie
		m_pDoc->m_TableRowsCount=rowsCount;
		m_pDoc->m_TableColumnsCount=columnsCount;
		m_pDoc->m_TableMPI=new BYTE[rowsCount*columnsCount];
		for(k=0;k<rowsCount*columnsCount;k++)m_pDoc->m_TableMPI[k]=tableMPI[k];
		m_pDoc->m_TableFunctionPoint=new DWORD[columnsCount];
		for(k=0;k<columnsCount;k++)m_pDoc->m_TableFunctionPoint[k]=FunctionPoint[k];
		m_pDoc->m_TableImplikants=new SImplikant[rowsCount];
		for(k=0;k<rowsCount;k++)CopyImplikant(&m_pDoc->m_TableImplikants[k],&prosteImplikanty[k]);
		m_pDoc->m_TableRowsSelect=new BYTE[rowsCount];
		for(k=0;k<rowsCount;k++)m_pDoc->m_TableRowsSelect[k]=rowsSelect[k];
	}

	//vyplnenie implikantov
	SImplikant *dnfImplikant;
	count=0;
	for(k=0;k<rowsCount;k++)if((rowsSelect[k] & ITEM_IS_DNF)!=0)count++;
	dnfImplikant=new SImplikant[count];
	count=0;
	for(k=0;k<rowsCount;k++)if((rowsSelect[k] & ITEM_IS_DNF)!=0){		
		dnfImplikant[count++]=prosteImplikanty[k];
	}else KillImplikant(&prosteImplikanty[k]);
	for(k=0;k<count;k++)prosteImplikanty[k]=dnfImplikant[k];
	*pocetProstychImplikantov=count;
	SAFE_DELETE(dnfImplikant);

	//!!! NEZABUDNI PO SEBE UVOLNIT PAMAT !!!
	SAFE_DELETE(tableMPI);
	SAFE_DELETE(rowsSelect);
	SAFE_DELETE(columnsSelect);
	SAFE_DELETE(FunctionPoint);
}
