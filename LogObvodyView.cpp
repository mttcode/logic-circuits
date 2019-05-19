// LogObvodyView.cpp : implementation of the CLogObvodyView class
//

#include "stdafx.h"
#include "LogObvody.h"

#include "LogObvodyDoc.h"
#include "LogObvodyView.h"

#include "TableOfBoolFunctionDlg.h"
#include "OutputView.h"
#include "StatusDlg.h"
#include "ProgramHelpDlg.h"

#include "MultipagePreview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyView
//paramtre vykreslovania
#define OFFSET_VAR				100	//medzera medzi kontaktnymi linkami
#define ELM_SIZE				50	//velkost clena (polovica)
#define ELM_OFFSET_VAR_SIZE		26	//vzdialenost medzi kontaktmi pri clene (parne cislo)
#define OFS_INVERT_LINE			10	//medzera medzi negujucimi linajkami
#define TABLE_SPACE				25	//velkost okrajov v tabulke
#define PRINT_BORDER			100	//okraj strany pri tlaci

IMPLEMENT_DYNCREATE(CLogObvodyView, CScrollView)

BEGIN_MESSAGE_MAP(CLogObvodyView, CScrollView)
	//{{AFX_MSG_MAP(CLogObvodyView)
	ON_COMMAND(ID_TABLE_CHANGE, OnTableChange)
	ON_COMMAND(ID_VIEW_SETTINGS, OnViewSettings)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(IDC_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_HELP_PROGRAM, OnHelpProgram)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyView construction/destruction

CLogObvodyView::CLogObvodyView()
{
	// TODO: add construction code here
	m_QuinMcCluskey=new CQuinMcCluskey(this);

	//nastavenie fontov
	//normal font
	m_FontHeight[0]=50;
	m_FontWeight[0]=0;

	//bold font
	m_FontHeight[1]=50;
	m_FontWeight[1]=FW_BOLD;

	//higher bold font
	m_FontHeight[2]=70;
	m_FontWeight[2]=FW_BOLD;
	CFont font;
	//int m_FontHeight=50;

	int k;
	for(k=0;k<FONT_COUNT;k++){
		VERIFY(m_Fonts[k].CreateFont(
		   m_FontHeight[k],           // nHeight
		   0,                         // nWidth
		   0,                         // nEscapement
		   0,                         // nOrientation
		   m_FontWeight[k],           // nWeight
		   FALSE,                     // bItalic
		   FALSE,                     // bUnderline
		   0,                         // cStrikeOut
		   DEFAULT_CHARSET,           // nCharSet
		   OUT_DEFAULT_PRECIS,        // nOutPrecision
		   CLIP_DEFAULT_PRECIS,       // nClipPrecision
		   DEFAULT_QUALITY,           // nQuality
		   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		   "Arial"));                 // lpszFacename
	}
	m_FontHeight[2]+=10;

	//m_ViewHelp=TRUE;
	m_ViewTableFunctionValues=TRUE;
	m_ViewCalculateSteps=TRUE;
	m_ViewTableMPI=TRUE;
	m_View_AND_OR_INVERT_Scheme=TRUE;
	m_View_NAND_Scheme=TRUE;
	m_View_NOR_Scheme=TRUE;
	m_ViewFunctionBool=TRUE;
	m_ViewFunctionSheffer=TRUE;
	m_ViewFunctionPierce=TRUE;

	m_SizeTableFunctionValues=CSize(0,0);
	m_SizeCalculateSteps=CSize(0,0);
	m_SizeTableMPI=CSize(0,0);
	m_Size_AND_OR_INVERT_Scheme=CSize(0,0);
	m_Size_NAND_Scheme=CSize(0,0);
	m_Size_NOR_Scheme=CSize(0,0);
	m_SizeFunctionBool=CSize(0,0);
	m_SizeFunctionSheffer=CSize(0,0);
	m_SizeFunctionPierce=CSize(0,0);

	m_Calculating=FALSE;
	/*
	//nacitanie helpu
	m_HelpTextCount=0;
	FILE *f;
	char c;
	f=fopen("help.bin","rb");
	if(f!=NULL){
		while(!feof(f)){
			fread(&c,sizeof(char),1,f);
			if(m_HelpTextCount<10000){
				m_HelpText[m_HelpTextCount]=c ^ 666;
				m_HelpTextCount++;
			}
		}
		fclose(f);
	}
	*/
}

CLogObvodyView::~CLogObvodyView()
{
	SAFE_DELETE(m_QuinMcCluskey);
	int k;
	for(k=0;k<FONT_COUNT;k++)m_Fonts[k].DeleteObject();
}

BOOL CLogObvodyView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyView drawing

void CLogObvodyView::TextOut(CDC* pDC,char *str,int *posX,int *posY)
{
	pDC->TextOut(*posX,(*posY),str);
	CSize textSize;
	textSize=pDC->GetTextExtent(str);
	(*posX)+=textSize.cx;
	CSize sizeView=GetTotalSize();
	if((*posX)>sizeView.cx){
		sizeView.cx=(*posX)+50;
		//SetScrollSizes(MM_LOMETRIC, sizeView);
	}
}

CSize CLogObvodyView::DrawImplikant(CDC* pDC,int startX,int startY,PSImplikant impl)
{
	CSize size;
	size.cx=size.cy=0;
	CLogObvodyDoc* pDoc = GetDocument();
	int i;
	char str[256];
	BOOL firstValue=TRUE;

	for(i=(int)pDoc->m_VariablesCount-1;i>=0;i--){
		if(impl->binValues[i]!=BIT_UNDEFINED){
			int dx=startX,dy=startY;
			if(!firstValue){
				strcpy(str," . ");
				TextOut(pDC,str,&startX,&startY);
			}else firstValue=FALSE;

			//je to premenna
			sprintf(str,"%c",pDoc->m_VariablesCount-i-1+'a');
			if(impl->binValues[i]==BIT_ZERO)pDC->MoveTo(startX,startY);
			TextOut(pDC,str,&startX,&startY);
			if(impl->binValues[i]==BIT_ZERO)pDC->LineTo(startX,startY);

			size.cx+=startX-dx;
			size.cy+=startY-dy;
		}
	}

	return size;
}

CSize CLogObvodyView::DrawImplicent(CDC* pDC,int startX,int startY,PSImplikant impl)
{
	CSize size;
	size.cx=size.cy=0;
	CLogObvodyDoc* pDoc = GetDocument();
	int i;
	char str[256];
	BOOL firstValue=TRUE;

	int aktIndex=0;
	//CPoint startPoint(startX,startY);
	for(i=(int)pDoc->m_VariablesCount-1;i>=0;i--){
		if(impl->binValues[i]!=BIT_UNDEFINED){
			aktIndex++;
			int dx=startX,dy=startY;
			if(!firstValue){
				strcpy(str," + ");
				TextOut(pDC,str,&startX,&startY);
			}else{
				//if(indexCount>1)TextOut(pDC,"( ",&startX,&startY);
				firstValue=FALSE;
			}

			//je to premenna
			sprintf(str,"%c",pDoc->m_VariablesCount-i-1+'a');
			if(impl->binValues[i]==BIT_ONE)pDC->MoveTo(startX,startY);
			TextOut(pDC,str,&startX,&startY);
			if(impl->binValues[i]==BIT_ONE)pDC->LineTo(startX,startY);

			//if(indexCount>1 && aktIndex==indexCount)TextOut(pDC," )",&startX,&startY);

			size.cx+=startX-dx;
			size.cy+=startY-dy;
		}
	}

	return size;
}

CSize CLogObvodyView::CalculateExtentImplikant(CDC* pDC,PSImplikant impl)
{
	CLogObvodyDoc* pDoc = GetDocument();
	int i;
	char str[256],tmpstr[256];
	BOOL firstValue=TRUE;

	str[0]=0;
	for(i=(int)pDoc->m_VariablesCount-1;i>=0;i--){
		if(impl->binValues[i]!=BIT_UNDEFINED){
			if(!firstValue){
				strcat(str," . ");
			}else firstValue=FALSE;

			//je to premenna
			sprintf(tmpstr,"x%d",pDoc->m_VariablesCount-i);
			strcat(str,tmpstr);
		}
	}

	return pDC->GetTextExtent(str);
}

void CLogObvodyView::DrawRect(CDC *pDC,CRect rect)
{
	pDC->MoveTo(rect.left,rect.top);
	pDC->LineTo(rect.right,rect.top);
	pDC->LineTo(rect.right,rect.bottom);
	pDC->LineTo(rect.left,rect.bottom);
	pDC->LineTo(rect.left,rect.top);
}

void CLogObvodyView::OnDraw(CDC* pDC)
{
	CLogObvodyDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	// TODO: add draw code for native data here
	CFont* def_font;

	pDC->SetBkMode(TRANSPARENT);
	CSize itemSize;
	CSize sizeView=GetTotalSize();
	sizeView=CSize(0,0);

	//vypis minimalnej fcie
	int actPositionX,actPositionY; //delta posun pre znak
	actPositionX=100;
	actPositionY=30;
	//int maxWidth2=0;	//zarovanie podstatnych impliknatov - vypis
/*
	def_font = pDC->SelectObject(&m_Fonts[0]);
	actPositionY+=m_FontHeight[0];
	pDC->TextOut(0,0,"XXX ¾šèžýáíé XXX");
	pDC->SelectObject(def_font);
*/
	/*
	if(m_ViewHelp){
		itemSize=DrawHelp(pDC,actPositionX,actPositionY);
		actPositionY+=itemSize.cy;
	}
	*/

	if(m_Calculating){
		def_font = pDC->SelectObject(&m_Fonts[2]);
		pDC->TextOut(actPositionX,-actPositionY,"Prebieha výpoèet");
		pDC->SelectObject(def_font);
		return;
	}

	if(pDoc->m_BoolFunctionValues==NULL){
		def_font = pDC->SelectObject(&m_Fonts[2]);
		pDC->SetTextColor(RGB(255,0,0));
		pDC->TextOut(actPositionX,-actPositionY,"Nie je zadaná booleovská funkcia");
		pDC->SetTextColor(RGB(0,0,0));
		actPositionY+=m_FontHeight[2];
		pDC->SelectObject(def_font);
	}else{
		if(m_ViewTableFunctionValues){
			//tabulka funkcnych hodnot
			itemSize=DrawTableOfFunctionValues(pDC,actPositionX,actPositionY);
			m_SizeTableFunctionValues=itemSize;
			actPositionY+=itemSize.cy;
			if(itemSize.cx+actPositionX>sizeView.cx)sizeView.cx=itemSize.cx+actPositionX;
		}
		 
		if(m_ViewFunctionBool && pDoc->m_ImplikantsCount==0){
			def_font = pDC->SelectObject(&m_Fonts[0]);
			actPositionY+=m_FontHeight[0];
			pDC->TextOut(actPositionX,-actPositionY,"Zadaný špecifický prípad - nemá zmysel optimalizácia");
			actPositionY+=m_FontHeight[0];			
			if(pDoc->m_FunctionFlag==BIT_ZERO){
				pDC->SelectObject(&m_Fonts[1]);
				pDC->TextOut(actPositionX,-actPositionY,"y = ");
				itemSize=pDC->GetTextExtent("y = ");
				pDC->SelectObject(&m_Fonts[0]);
				pDC->TextOut(actPositionX+itemSize.cx,-actPositionY,"0");
			}
			if(pDoc->m_FunctionFlag==BIT_ONE){
				pDC->SelectObject(&m_Fonts[1]);
				pDC->TextOut(actPositionX,-actPositionY,"y = ");
				itemSize=pDC->GetTextExtent("y = ");
				pDC->SelectObject(&m_Fonts[0]);
				pDC->TextOut(actPositionX+itemSize.cx,-actPositionY,"1");
			}
			actPositionY+=2*m_FontHeight[0];
			pDC->SelectObject(def_font);
		}

		if(m_ViewCalculateSteps && pDoc->m_ImplikantsCount>0){
			//postup minimalizacie
			itemSize=DrawCalculateSteps(pDC,actPositionX,actPositionY);
			m_SizeCalculateSteps=itemSize;
			actPositionY+=itemSize.cy;
			if(itemSize.cx+actPositionX>sizeView.cx)sizeView.cx=itemSize.cx+actPositionX;
		}

		if(m_ViewTableMPI && pDoc->m_ImplikantsCount>0){
			//tabulka pokrytia funkcie
			itemSize=DrawTableMPI(pDC,actPositionX,actPositionY);
			m_SizeTableMPI=itemSize;
			actPositionY+=itemSize.cy;
			if(itemSize.cx+actPositionX>sizeView.cx)sizeView.cx=itemSize.cx+actPositionX;
		}

		//AND_OR_INVERT schema
		if(m_ViewFunctionBool && pDoc->m_ImplikantsCount>0){
			itemSize=DrawFunctionAND_OR_INVERT(pDC,actPositionX,actPositionY);
			m_SizeFunctionBool=itemSize;
			if(sizeView.cx<itemSize.cx)sizeView.cx=itemSize.cx;
			actPositionY+=itemSize.cy+m_FontHeight[0];
		}
		if(m_View_AND_OR_INVERT_Scheme && pDoc->m_ImplikantsCount>0){
			itemSize=Draw_AND_OR_INVERT(pDC,actPositionX,actPositionY);
			m_Size_AND_OR_INVERT_Scheme=itemSize;
			if(sizeView.cx<itemSize.cx)sizeView.cx=itemSize.cx;
			actPositionY+=itemSize.cy;
		}

		//NAND schema
		if(m_ViewFunctionSheffer && pDoc->m_ImplikantsCount>0){
			itemSize=DrawFunctionNAND(pDC,actPositionX,actPositionY);
			if(sizeView.cx<itemSize.cx)sizeView.cx=itemSize.cx;
			actPositionY+=itemSize.cy+m_FontHeight[0];
		}
		if(m_View_NAND_Scheme && pDoc->m_ImplikantsCount>0){
			itemSize=Draw_NAND(pDC,actPositionX,actPositionY);
			if(sizeView.cx<itemSize.cx)sizeView.cx=itemSize.cx;
			actPositionY+=itemSize.cy;
		}

		//NOR schema
		if(m_ViewFunctionPierce && pDoc->m_ImplikantsCount>0){
			itemSize=DrawFunctionNOR(pDC,actPositionX,actPositionY);
			if(sizeView.cx<itemSize.cx)sizeView.cx=itemSize.cx;
			actPositionY+=itemSize.cy+m_FontHeight[0];
		}
		if(m_View_NOR_Scheme && pDoc->m_ImplikantsCount>0){
			itemSize=Draw_NOR(pDC,actPositionX,actPositionY);
			if(sizeView.cx<itemSize.cx)sizeView.cx=itemSize.cx;
			actPositionY+=itemSize.cy;
		}
	}

	sizeView.cy=actPositionY;
	sizeView.cx+=actPositionX+500;
	//sizeView.cx=actPositionX;
	if(sizeView.cx<100)sizeView.cx=100;
	if(sizeView.cy<100)sizeView.cy=100;
	SetScrollSizes(MM_LOMETRIC, sizeView);

	//pDC->SelectObject(def_font);

	// Done with the font.  Delete the font object.
	//font.DeleteObject(); 
}
/*
CSize CLogObvodyView::DrawSlovakChar(CDC *pDC,int x,int y,char znak)
{
	char c=' ';
	switch(znak){
	case 'A':c='Á';break;
	case 'a':c='á';break;
	case 'C':c='È';break;
	case 'c':c='è';break;
	case 'D':c='Ï';break;
	case 'd':c='ï';break;
	case 'E':c='É';break;
	case 'e':c='é';break;
	case 'I':c='Í';break;
	case 'i':c='í';break;
	case 'L':c='¼';break;
	case 'l':c='¾';break;
	case 'N':c='Ò';break;
	case 'n':c='ò';break;
	case 'O':c='Ó';break;
	case 'o':c='ó';break;
	case 'S':c='Š';break;
	case 's':c='š';break;
	case 'T':c='';break;
	case 't':c='';break;
	case 'U':c='Ú';break;
	case 'u':c='ú';break;
	case 'Y':c='Ý';break;
	case 'y':c='ý';break;
	case 'Z':c='Ž';break;
	case 'z':c='ž';break;
	}
	pDC->TextOut(x,-y,&c,1);
	return pDC->GetTextExtent(&c,1);
}

CSize CLogObvodyView::DrawHelp(CDC *pDC,int ofsX,int ofsY)
{
	CLogObvodyDoc* pDoc = GetDocument();
	CSize schemeSize(0,0),itemSize;
	CRect clientRect;
	GetClientRect(&clientRect);
	pDC->DPtoLP(&clientRect);
	int k,i;
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);
	int startX=ofsX;
	for(k=0;k<m_HelpTextCount;k++){
		if(m_HelpText[k]=='<'){
			k++; //zahodim '<'
			if(m_HelpText[k]=='B')pDC->SelectObject(&m_Fonts[1]);
			if(m_HelpText[k]=='/'){
				k++;
				if(m_HelpText[k]=='B')pDC->SelectObject(&m_Fonts[0]);
				k++;
			}else k++;
			if(m_HelpText[k]=='>')k++;
		}
		if(m_HelpText[k]=='\n'){
			//odriadkovanie
			ofsY+=m_FontHeight[0];
			ofsX=startX;
			schemeSize.cy+=m_FontHeight[0];
		}else if(m_HelpText[k]>=32){
			if(m_HelpText[k]=='~'){
				//dlzen alebo makcen
				k++;
				itemSize=DrawSlovakChar(pDC,ofsX,ofsY,m_HelpText[k]);
				ofsX+=itemSize.cx+5;
			}else{
				//vypis znaku na aktualnu poziciu
				itemSize=pDC->GetTextExtent(&m_HelpText[k],1);
				pDC->TextOut(ofsX,-ofsY,&m_HelpText[k],1);
				ofsX+=itemSize.cx+5;
				if(schemeSize.cx<ofsX)schemeSize.cx=ofsX;
			}
			
			//umele zalomenie riadku
			if(m_HelpText[k]==' '){
				int width=0;
				for(i=k+1;i<m_HelpTextCount;i++){
					if(m_HelpText[i]<=32)break;
					itemSize=pDC->GetTextExtent(&m_HelpText[i],1);
					width+=itemSize.cx;
				}
				if(ofsX+width>clientRect.right-startX){
					ofsY+=m_FontHeight[0];
					ofsX=startX;
					schemeSize.cy+=m_FontHeight[0];
				}
			}
		}
	}
	pDC->SelectObject(defFont);
	return schemeSize;
}
*/
CSize CLogObvodyView::DrawTableOfFunctionValues(CDC *pDC,int ofsX,int ofsY)
{
	CLogObvodyDoc* pDoc = GetDocument();
	CSize schemeSize(0,0),itemSize;

	//sirky stlpcov
	int widthDE,widthVar,widthValue;
	int rowsHeight=m_FontHeight[0]+TABLE_SPACE;
	int k,i;
	char str[256];
	CFont *defFont=pDC->SelectObject(&m_Fonts[2]);

	pDC->TextOut(ofsX,-ofsY,"Tabu¾ka funkèných hodnôt:");
	ofsY+=m_FontHeight[2];
	pDC->SelectObject(&m_Fonts[0]);
	//vypocet sirky stlpca DE
	widthDE=0;
	for(k=0;k<(int)pDoc->m_ValuesCount;k++){
		sprintf(str,"%d",k);
		itemSize=pDC->GetTextExtent(str);
		if(itemSize.cx>widthDE)widthDE=itemSize.cx;
	}
	widthDE+=TABLE_SPACE*2;

	//vypocet sirky stlpcov s premennymi
	widthVar=0;
	for(k=0;k<(int)pDoc->m_VariablesCount;k++){
		sprintf(str,"%c",k+'a');
		itemSize=pDC->GetTextExtent(str);
		if(itemSize.cx>widthVar)widthVar=itemSize.cx;
	}
	widthVar+=TABLE_SPACE*2;

	//vypocet sirky stlpca s hodnotami fcie
	widthValue=widthDE;

	//nakreslenie tabulky
	CRect rect;
	pDC->SetTextColor(RGB(0,0,200));
	pDC->SelectObject(&m_Fonts[1]);
	for(k=0;k<(int)pDoc->m_VariablesCount+2;k++){
		if(k==0){
			//DE stlpec
			rect=CRect(ofsX,-ofsY,ofsX+widthDE,-(ofsY+rowsHeight));
			DrawRect(pDC,rect);
			pDC->DrawText("DE",&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		
		if(k>0 && k<=(int)pDoc->m_VariablesCount){
			//premenne
			rect=CRect(ofsX+widthDE+(k-1)*widthVar,-ofsY,ofsX+widthDE+k*widthVar,-(ofsY+rowsHeight));
			DrawRect(pDC,rect);
			sprintf(str,"%c",k-1+'a');
			pDC->DrawText(str,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		
		if(k==(int)pDoc->m_VariablesCount+1){
			//funkcna hodnota
			rect=CRect(ofsX+widthDE+pDoc->m_VariablesCount*widthVar,-ofsY,ofsX+widthDE+pDoc->m_VariablesCount*widthVar+widthValue,-(ofsY+rowsHeight));
			DrawRect(pDC,rect);
			pDC->DrawText("y",&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
	}
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SelectObject(&m_Fonts[0]);

	for(i=0;i<(int)pDoc->m_ValuesCount;i++){
		//if(i % 2)pDC->SetTextColor(RGB(0,0,255));else
		//	     pDC->SetTextColor(RGB(0,0,0));

		//DE stlpec
		rect=CRect(ofsX,-(ofsY+(i+1)*rowsHeight),ofsX+widthDE,-(ofsY+(i+2)*rowsHeight));
		DrawRect(pDC,rect);
		sprintf(str,"%d",i);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->DrawText(str,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		pDC->SetTextColor(RGB(0,0,0));
		
		//premenne
		for(k=0;k<(int)pDoc->m_VariablesCount;k++){
			rect=CRect(ofsX+widthDE+k*widthVar,-(ofsY+(i+1)*rowsHeight),ofsX+widthDE+(k+1)*widthVar,-(ofsY+(i+2)*rowsHeight));
			DrawRect(pDC,rect);
			sprintf(str,"%d",getBitValue(i,pDoc->m_VariablesCount-k-1));
			pDC->DrawText(str,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}

		//funkcna hodnota
		rect=CRect(ofsX+widthDE+pDoc->m_VariablesCount*widthVar,-(ofsY+(i+1)*rowsHeight),ofsX+widthDE+pDoc->m_VariablesCount*widthVar+widthValue,-(ofsY+(i+2)*rowsHeight));
		DrawRect(pDC,rect);
		if(pDoc->m_BoolFunctionValues[i]!=BIT_UNDEFINED)
			sprintf(str,"%d",pDoc->m_BoolFunctionValues[i]);else
			strcpy(str,"x");
		pDC->DrawText(str,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);		
	}
	pDC->SetTextColor(RGB(0,0,0));

	schemeSize.cx=widthDE+pDoc->m_VariablesCount*widthVar+widthValue;
	schemeSize.cy=rowsHeight*(pDoc->m_ValuesCount+1)+m_FontHeight[2];

	pDC->SelectObject(defFont);

	return schemeSize;
}

CSize CLogObvodyView::DrawCalculateSteps(CDC *pDC,int ofsX,int ofsY)
{
	CLogObvodyDoc* pDoc = GetDocument();
	CSize schemeSize(0,0),itemSize;
	int actPositionX=ofsX;
	int actPositionY=ofsY;
	PSIndexList indexList;
	int k,i,startY;
	char str[256],tmpstr[256];
	int maxWidth=0;		//zarovanavanie vzniknutych implikantov (binarny tvar)
	int maxWidth2=0;	//zarovanie podstatnych impliknatov - vypis
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);

	actPositionY+=m_FontHeight[0];

	pDC->SelectObject(&m_Fonts[2]);
	pDC->TextOut(actPositionX,-actPositionY,"Rozdelenie hodnôt do skupín pod¾a poètu jednièiek v binárnom tvare:");
	actPositionY+=m_FontHeight[2];
	pDC->SelectObject(&m_Fonts[0]);
	for(k=0;k<(int)pDoc->m_GroupValuesCount;k++)if(pDoc->m_GroupValues[k].countDe>0){
		sprintf(str,"%dx : ",k);
		for(i=0;i<(int)pDoc->m_GroupValues[k].countDe;i++){
			BinValue2Str(pDoc->m_GroupValues[k].de[i],tmpstr,pDoc->m_VariablesCount);
			sprintf(tmpstr,"%s (%d)",tmpstr,pDoc->m_GroupValues[k].de[i]);
			strcat(str,tmpstr);
			if(i<(int)pDoc->m_GroupValues[k].countDe-1)strcat(str,", ");
		}
		pDC->TextOut(actPositionX,-actPositionY,str);
		itemSize=pDC->GetTextExtent(str);
		if(schemeSize.cx<itemSize.cx)schemeSize.cx=itemSize.cx;
		actPositionY+=m_FontHeight[0];
	}
	if(pDoc->m_GroupValuesCount==0){
		pDC->TextOut(actPositionX,-actPositionY,"- zadaná funkcia neobsahuje žiaden jednièkový bod");
		actPositionY+=m_FontHeight[0];
	}
	actPositionY+=m_FontHeight[0];

	pDC->SelectObject(&m_Fonts[2]);
	pDC->TextOut(actPositionX,-actPositionY,"Spájanie skupín:");
	actPositionY+=m_FontHeight[2];
	pDC->SelectObject(&m_Fonts[0]);
	PSInterImplikant aktImplikants=pDoc->m_InterImplikants;
	while(aktImplikants!=NULL){
		startY=actPositionY;
		//vypis index, z ktorych vnikli dane implikanty
		for(k=0;k<(int)aktImplikants->countImplikants;k++){					
			indexList=aktImplikants->implikants[k].indexList;
			str[0]=0;
			while(indexList!=NULL){
				for(i=0;i<(int)indexList->indexCount;i++){
					sprintf(tmpstr,"(%d)",indexList->index[i]);
					strcat(str,tmpstr);
					if(i<(int)indexList->indexCount-1)strcat(str," - ");
				}
				indexList=indexList->next;
			}
			pDC->TextOut(actPositionX,-actPositionY,str);
			itemSize=pDC->GetTextExtent(str);
			if(schemeSize.cx<itemSize.cx)schemeSize.cx=itemSize.cx;
			actPositionY+=m_FontHeight[0];
			//itemSize=pDC->GetTextExtent(str);
			if(itemSize.cx>maxWidth)maxWidth=itemSize.cx;
		}

		//vypis vniknutych implikantov (koli zarovnavaniu je to 2 fazove)
		actPositionY=startY;
		for(k=0;k<(int)aktImplikants->countImplikants;k++){					
			MergeImplikants2Str(&aktImplikants->implikants[k],pDoc->m_VariablesCount,str);
			itemSize=pDC->GetTextExtent(str);
			if(itemSize.cx>maxWidth2)maxWidth2=itemSize.cx;
			pDC->TextOut(actPositionX+maxWidth+30,-actPositionY,str);
			actPositionY+=m_FontHeight[0];
		}

		//vypis podstatnych implikantov
		actPositionY=startY;
		for(k=0;k<(int)aktImplikants->countImplikants;k++){
			if(aktImplikants->implikants[k].flag==FLAG_UNUSED){
				itemSize=DrawImplikant(pDC,actPositionX+maxWidth+30+maxWidth2+30,-actPositionY,&aktImplikants->implikants[k]);
				if(schemeSize.cx<itemSize.cx+actPositionX+maxWidth+30+maxWidth2+30+50)schemeSize.cx=itemSize.cx+actPositionX+maxWidth+30+maxWidth2+30+50;
			}
			actPositionY+=m_FontHeight[0];
		}

		aktImplikants=aktImplikants->next;
		pDC->TextOut(actPositionX,-actPositionY,"----------------------------------------");
		actPositionY+=m_FontHeight[0];
	}
	schemeSize.cx-=ofsX;
	schemeSize.cy=actPositionY-ofsY;
	pDC->SelectObject(defFont);
	return schemeSize;
}

CSize CLogObvodyView::DrawTableMPI(CDC *pDC,int ofsX,int ofsY)
{
	CPen bluePen,blackPen,*def_pen;
	bluePen.CreatePen(PS_SOLID,1,RGB(0,0,255));
	blackPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	def_pen=pDC->SelectObject(&blackPen);

	CLogObvodyDoc* pDoc = GetDocument();
	CSize schemeSize(0,0),itemSize;
	int actPositionX=ofsX;
	int actPositionY=ofsY;
	int k,i,maxWidth;
	char str[256];
	int startY;	
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);

	//tabulka pokrytia funkcie
	CSize rectSize;	//rozmer policka
	rectSize.cx=rectSize.cy=0;
	actPositionY+=m_FontHeight[0];
	pDC->SelectObject(&m_Fonts[2]);
	pDC->TextOut(actPositionX,-actPositionY,"Tabu¾ka pokrytia:");
	actPositionY+=m_FontHeight[2];
	pDC->SelectObject(&m_Fonts[0]);
	maxWidth=0;
	startY=actPositionY;
	rectSize.cy=m_FontHeight[0]+2*TABLE_SPACE;
	for(k=0;k<(int)pDoc->m_TableRowsCount;k++){
		itemSize=CalculateExtentImplikant(pDC,&pDoc->m_TableImplikants[k]);
		//if((pDoc->m_TableRowsSelect[k] & ITEM_IS_DNF)!=0)pDC->TextOut(actPositionX-m_FontHeight,actPositionY,"*");
		if(itemSize.cx>maxWidth)maxWidth=itemSize.cx;
		//actPositionY+=rectSize.cy;
	}
	maxWidth+=2*TABLE_SPACE;
	for(k=0;k<(int)pDoc->m_TableColumnsCount;k++){
		sprintf(str,"%d",pDoc->m_TableFunctionPoint[k]);
		itemSize=pDC->GetTextExtent(str);
		if(itemSize.cx>rectSize.cx)rectSize.cx=itemSize.cx;
	}
	rectSize.cx+=2*TABLE_SPACE;

	//relativna pozicia od (actPositionX,actPositionY)
	//prvy riadok -> dekadicke ekvivalenty
	CRect actTableItem;
	for(k=0;k<(int)pDoc->m_TableColumnsCount;k++){
		actTableItem=CRect(actPositionX+maxWidth+k*rectSize.cx,-actPositionY,actPositionX+maxWidth+(k+1)*rectSize.cx,-(actPositionY+rectSize.cy));
		DrawRect(pDC,actTableItem);
		sprintf(str,"%d",pDoc->m_TableFunctionPoint[k]);
		pDC->DrawText(str,&actTableItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}

	actPositionY+=rectSize.cy;
	//vykreslenie tabulky
	for(k=0;k<(int)pDoc->m_TableRowsCount;k++){
		//prvy stlpec -> implikanty
		actTableItem=CRect(actPositionX,-actPositionY,actPositionX+maxWidth,-(actPositionY+rectSize.cy));
		DrawRect(pDC,actTableItem);
		if((pDoc->m_TableRowsSelect[k] & ITEM_IS_DNF)!=0){
			def_pen=pDC->SelectObject(&bluePen);
			pDC->SetTextColor(0x00ff0000);
		}else{
			def_pen=pDC->SelectObject(&blackPen);
			pDC->SetTextColor(0x00000000);
		}
		DrawImplikant(pDC,actPositionX+TABLE_SPACE,-(actPositionY+TABLE_SPACE),&pDoc->m_TableImplikants[k]);
		pDC->SetTextColor(0x00000000);
		pDC->SelectObject(def_pen);
		for(i=0;i<(int)pDoc->m_TableColumnsCount;i++){
			actTableItem=CRect(actPositionX+maxWidth+i*rectSize.cx,-actPositionY,actPositionX+maxWidth+(i+1)*rectSize.cx,-(actPositionY+rectSize.cy));
			DrawRect(pDC,actTableItem);
			if(pDoc->m_TableMPI[i+k*pDoc->m_TableColumnsCount]==1)pDC->DrawText("*",&actTableItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		actPositionY+=rectSize.cy;
	}
	itemSize.cx=maxWidth+rectSize.cx*pDoc->m_TableColumnsCount;
	schemeSize.cx=itemSize.cx-ofsX;
	//actPositionY+=rectSize.cy*(pDoc->m_TableRowsCount+1);
	actPositionY+=m_FontHeight[0];
	schemeSize.cy=actPositionY-ofsY;

	pDC->SelectObject(def_pen);

	bluePen.DeleteObject();
	blackPen.DeleteObject();
	pDC->SelectObject(defFont);
	return schemeSize;
}

void CLogObvodyView::DrawLogicalElement(CDC *pDC,int ofsX,int ofsY,int contactsCount,char elmType,BOOL invert)
{
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);
	CRect rectElm;
	int width,height;
	width=ELM_SIZE*2;
	height=ELM_SIZE*2+(contactsCount-1)*ELM_OFFSET_VAR_SIZE;
	rectElm.left=ofsX;
	rectElm.right=ofsX+width;
	rectElm.top=-ofsY;
	rectElm.bottom=-ofsY-height;
	DrawRect(pDC,rectElm);
	CSize charSize;
	charSize=pDC->GetTextExtent(&elmType,1);
	pDC->TextOut(rectElm.right-charSize.cx-2,rectElm.top-2,&elmType,1);
	if(invert){
		CRect ellipseRect(rectElm.right,(rectElm.top+rectElm.bottom)/2+ELM_OFFSET_VAR_SIZE/2,rectElm.right+ELM_OFFSET_VAR_SIZE,(rectElm.top+rectElm.bottom)/2-ELM_OFFSET_VAR_SIZE/2);
		pDC->Ellipse(&ellipseRect);
	}else{
		pDC->MoveTo(rectElm.right,(rectElm.top+rectElm.bottom)/2);
		pDC->LineTo(rectElm.right+ELM_OFFSET_VAR_SIZE,(rectElm.top+rectElm.bottom)/2);
	}

	int k;
	for(k=0;k<contactsCount;k++){
		pDC->MoveTo(rectElm.left,rectElm.top-ELM_SIZE-k*ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(rectElm.left-ELM_OFFSET_VAR_SIZE,rectElm.top-ELM_SIZE-k*ELM_OFFSET_VAR_SIZE);
	}
	pDC->SelectObject(defFont);
}

void CLogObvodyView::DrawConector(CDC *pDC,int x,int y)
{
	CBrush blackBrush,*defBrush;
	blackBrush.CreateSolidBrush(RGB(0,0,0));
	defBrush=pDC->SelectObject(&blackBrush);
	CRect rect;
	rect.left=x-ELM_OFFSET_VAR_SIZE/2+1;
	rect.right=x+ELM_OFFSET_VAR_SIZE/2;
	rect.top=y+ELM_OFFSET_VAR_SIZE/2-1;
	rect.bottom=y-ELM_OFFSET_VAR_SIZE/2;
	pDC->Ellipse(&rect);
	pDC->SelectObject(defBrush);
}

CSize CLogObvodyView::Draw_AND_OR_INVERT(CDC *pDC,int ofsX,int ofsY)
{
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);
	CLogObvodyDoc* pDoc = GetDocument();
	BOOL useInvert[32];	//ci danu premennu potrebujem aj negovanu a ci vobec potrebujem invertory
	CSize schemeSize(0,0);
	int k,i;
	CSize strSize;
	char str[256];
	CPoint conectorsPoint[100];	//jednotlive konektory
	int conectorsPointCount=0;	//pocet konektorov

	pDC->SelectObject(&m_Fonts[2]);
	pDC->TextOut(ofsX,-ofsY,"Kontaktná schéma v Booleovej algebre:");
	ofsY+=m_FontHeight[2]+m_FontHeight[2]/2;
	pDC->SelectObject(&m_Fonts[0]);

	for(k=0;k<32;k++)useInvert[k]=FALSE;
	for(k=0;k<(int)pDoc->m_ImplikantsCount;k++){
		for(i=(int)pDoc->m_VariablesCount-1;i>=0;i--){
			if(pDoc->m_Implikants[k].binValues[i]==0)useInvert[i]=TRUE;
		}
	}
	DWORD implikantIndex=0,valuesCount;

	//nakreslenie INVERT a AND clenov
	valuesCount=0;
	implikantIndex=0;
	int aktY=ofsY+OFFSET_VAR;
	POINT elmOR;	//ked je roh clena OR
	elmOR.x=ofsX+(pDoc->m_VariablesCount-1)*OFFSET_VAR*2+OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR*2+ELM_OFFSET_VAR_SIZE*(pDoc->m_ImplikantsCount-1);
	elmOR.y=ofsY+OFFSET_VAR;
	CPoint elmAND_Point;	//ak je iba jeden AND clen -> toto je suradnica jeho vystupu
	for(k=0;k<(int)pDoc->m_VariablesCount;k++)if(useInvert[k]){
		//vykreslenie clena INVERT
		DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR,aktY,1,'1',TRUE);
		if(aktY+2*ELM_SIZE>schemeSize.cy)schemeSize.cy=aktY+2*ELM_SIZE;
		//napojenie clena
		pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR,-aktY-ELM_SIZE);
		//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY-ELM_SIZE);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);

		aktY+=ELM_SIZE*2+OFFSET_VAR;
		if(implikantIndex<pDoc->m_ImplikantsCount){
			DWORD tmpIndex;
			if(pDoc->m_Implikants[implikantIndex].count>1){
				//vykreslenie clena AND
				DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY-ELM_SIZE,pDoc->m_Implikants[implikantIndex].count,'&',FALSE);
				if(aktY+ELM_SIZE+pDoc->m_Implikants[implikantIndex].count*ELM_OFFSET_VAR_SIZE>(DWORD)schemeSize.cy)schemeSize.cy=aktY+ELM_SIZE+pDoc->m_Implikants[implikantIndex].count*ELM_OFFSET_VAR_SIZE;
				elmAND_Point=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
				//napojenie clena
				tmpIndex=0;
				for(i=0;i<(int)pDoc->m_VariablesCount;i++)if(pDoc->m_Implikants[implikantIndex].binValues[i]!=BIT_UNDEFINED){
					if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ZERO){
						pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
						//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
						conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					}
					if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ONE){
						pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
						//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
						conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					}
					pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					tmpIndex++;
				}
			}else{
				//je to obycajna premenna (priama alebo negovana)
				for(i=0;i<(int)pDoc->m_VariablesCount;i++)if(pDoc->m_Implikants[implikantIndex].binValues[i]!=BIT_UNDEFINED){
					if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ZERO){
						pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
						//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
						conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
					}
					if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ONE){
						pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
						//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
						conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
					}
					pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
					elmAND_Point=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
					if(elmAND_Point.y>schemeSize.cy)schemeSize.cy=elmAND_Point.y;
					break;
				}
			}
			if(pDoc->m_ImplikantsCount>=2){
				//napojenie vystupu na OR clen
				int tmpX,tmpY;
				tmpX=ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE;
				tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2;
				pDC->MoveTo(tmpX,tmpY);
				pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,tmpY);
				pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
				pDC->LineTo(elmOR.x,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
			}
			aktY+=OFFSET_VAR+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE;
			implikantIndex++;
		}
		valuesCount++;
	}
	while(implikantIndex<pDoc->m_ImplikantsCount){
		DWORD tmpIndex;
		if(pDoc->m_Implikants[implikantIndex].count>1){
			//vykreslenie clena AND
			DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY,pDoc->m_Implikants[implikantIndex].count,'&',FALSE);
			if(aktY+2*ELM_SIZE+pDoc->m_Implikants[implikantIndex].count*ELM_OFFSET_VAR_SIZE>(DWORD)schemeSize.cy)schemeSize.cy=aktY+2*ELM_SIZE+pDoc->m_Implikants[implikantIndex].count*ELM_OFFSET_VAR_SIZE;
			elmAND_Point=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
			//napojenie clena
			tmpIndex=0;
			for(i=0;i<(int)pDoc->m_VariablesCount;i++)if(pDoc->m_Implikants[implikantIndex].binValues[i]!=BIT_UNDEFINED){
				if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ZERO){
					pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
					//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
					conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				}
				if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ONE){
					pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
					//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
					conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				}
				pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				tmpIndex++;
			}
		}else{
			//je to obycajna premenna (priama alebo negovana)
			for(i=0;i<(int)pDoc->m_VariablesCount;i++)if(pDoc->m_Implikants[implikantIndex].binValues[i]!=BIT_UNDEFINED){
				if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ZERO){
					pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
					//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
					conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
				}
				if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ONE){
					pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
					//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
					conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
				}
				pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
				elmAND_Point=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
				if(-elmAND_Point.y>schemeSize.cy)schemeSize.cy=-elmAND_Point.y;
				break;
			}
		}
		if(pDoc->m_ImplikantsCount>=2){
			//napojenie vystupu na OR clen
			int tmpX,tmpY;
			tmpX=ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE;
			tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE;
			pDC->MoveTo(tmpX,tmpY);
			pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,tmpY);
			pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
			pDC->LineTo(elmOR.x,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
		}
		aktY+=2*OFFSET_VAR+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE;
		implikantIndex++;
	}
	if(pDoc->m_ImplikantsCount>=2){
		//je potrebne nakreslit aj OR clen
		DrawLogicalElement(pDC,elmOR.x,elmOR.y,pDoc->m_ImplikantsCount,'1',FALSE);
		pDC->MoveTo(elmOR.x+ELM_OFFSET_VAR_SIZE+OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCount-1)*ELM_OFFSET_VAR_SIZE)/2);
		pDC->LineTo(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCount-1)*ELM_OFFSET_VAR_SIZE)/2);
		strSize=pDC->GetTextExtent("y");
		pDC->TextOut(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR-strSize.cx,-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCount-1)*ELM_OFFSET_VAR_SIZE)/2+strSize.cy+ELM_OFFSET_VAR_SIZE,"y");
		schemeSize.cx=elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR-strSize.cx-ofsX;
		int tmpNum=-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCount-1)*ELM_OFFSET_VAR_SIZE)/2+strSize.cy+ELM_OFFSET_VAR_SIZE;
		if(-tmpNum>schemeSize.cy)schemeSize.cy=-tmpNum;
	}else{
		//mam iba 1 AND clen -> jeho vystup je vystup fcie
		pDC->MoveTo(elmAND_Point.x,elmAND_Point.y);
		pDC->LineTo(elmAND_Point.x+OFFSET_VAR,elmAND_Point.y);
		strSize=pDC->GetTextExtent("y");
		pDC->TextOut(elmAND_Point.x+OFFSET_VAR-strSize.cx,elmAND_Point.y+strSize.cy+ELM_OFFSET_VAR_SIZE/2,"y");
		schemeSize.cx=elmAND_Point.x+OFFSET_VAR-strSize.cx-ofsX;
		if(-elmAND_Point.y>schemeSize.cy)schemeSize.cy=-elmAND_Point.y;
	}

	schemeSize.cy-=ofsY;

	CPen bluePen,blackPen,*defPen;
	bluePen.CreatePen(PS_SOLID,0,RGB(0,0,255));
	blackPen.CreatePen(PS_SOLID,0,RGB(0,0,0));
	defPen=pDC->SelectObject(&blackPen);

	//nakreslenie zvyslych kontaktnych liniek
	for(k=0;k<(int)pDoc->m_VariablesCount;k++){
		pDC->SelectObject(&bluePen);
		pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-OFFSET_VAR);
		pDC->LineTo(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-schemeSize.cy-ELM_SIZE);
		pDC->SelectObject(&blackPen);
		sprintf(str,"%c",pDoc->m_VariablesCount-k-1+'a');
		pDC->TextOut(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,str);
		if(useInvert[k]){
			//tuto premennu potrebujem aj v negovanom stave
			pDC->SelectObject(&bluePen);
			pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-OFFSET_VAR);
			pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-schemeSize.cy-ELM_SIZE);
			pDC->SelectObject(&blackPen);
			pDC->TextOut(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,str);
			strSize=pDC->GetTextExtent(str);
			pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2);
			pDC->LineTo(strSize.cx+ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2);
		}
	}

	for(k=0;k<conectorsPointCount;k++)DrawConector(pDC,conectorsPoint[k].x,conectorsPoint[k].y);
	pDC->SelectObject(defPen);

	schemeSize.cy+=OFFSET_VAR+m_FontHeight[2]+m_FontHeight[2]/2;
	pDC->SelectObject(defFont);

	return schemeSize;
}

CSize CLogObvodyView::Draw_NAND(CDC *pDC,int ofsX,int ofsY)
{
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);
	CLogObvodyDoc* pDoc = GetDocument();
	BOOL useInvert[32];	//ci danu premennu potrebujem aj negovanu a ci vobec potrebujem invertory
	CSize schemeSize(0,0);
	int k,i;
	CSize strSize;
	char str[256];
	CPoint conectorsPoint[100];	//jednotlive konektory
	int conectorsPointCount=0;	//pocet konektorov

	pDC->SelectObject(&m_Fonts[2]);
	pDC->TextOut(ofsX,-ofsY,"Kontaktná schéma v Shefferovej algebre:");
	ofsY+=m_FontHeight[2]+m_FontHeight[2]/2;
	pDC->SelectObject(&m_Fonts[0]);

	for(k=0;k<32;k++)useInvert[k]=FALSE;
	for(k=0;k<(int)pDoc->m_ImplikantsCount;k++){
		for(i=(int)pDoc->m_VariablesCount-1;i>=0;i--){
			if(pDoc->m_Implikants[k].binValues[i]==0)useInvert[i]=TRUE;
		}
	}

	DWORD implikantIndex=0,valuesCount;

	//nakreslenie INVERT a AND clenov
	valuesCount=0;
	implikantIndex=0;
	int aktY=ofsY+OFFSET_VAR;
	POINT elmOR;	//ked je roh clena OR
	elmOR.x=ofsX+(pDoc->m_VariablesCount-1)*OFFSET_VAR*2+OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR*2+ELM_OFFSET_VAR_SIZE*(pDoc->m_ImplikantsCount-1);
	elmOR.y=ofsY+OFFSET_VAR;
	CPoint elmAND_Point;	//ak je iba jeden AND clen -> toto je suradnica jeho vystupu
	for(k=0;k<(int)pDoc->m_VariablesCount;k++)if(useInvert[k]){
		//vykreslenie clena INVERT
		DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR,aktY,2,'&',TRUE);
		if(aktY+2*ELM_SIZE+ELM_OFFSET_VAR_SIZE>schemeSize.cy)schemeSize.cy=aktY+2*ELM_SIZE+ELM_OFFSET_VAR_SIZE;
		//napojenie clena
		pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR,-aktY-ELM_SIZE);
		//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		
		//napojenie 2. vstupu
		pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR-OFFSET_VAR/2,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR-OFFSET_VAR/2,-aktY-ELM_SIZE);
		//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR-OFFSET_VAR/2,-aktY-ELM_SIZE);
		conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR-OFFSET_VAR/2,-aktY-ELM_SIZE);

		pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE/2);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE/2);
		//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE/2);
		conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE/2);

		aktY+=ELM_SIZE*2+OFFSET_VAR;
		if(implikantIndex<pDoc->m_ImplikantsCount){
			DWORD tmpIndex;

			//vykreslenie clena AND
			if(pDoc->m_Implikants[implikantIndex].count>1){
				DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY-ELM_SIZE,pDoc->m_Implikants[implikantIndex].count,'&',TRUE);
			}else{
				DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY-ELM_SIZE,2,'&',TRUE);
			}
			if(aktY+ELM_SIZE+pDoc->m_Implikants[implikantIndex].count*ELM_OFFSET_VAR_SIZE>(DWORD)schemeSize.cy)schemeSize.cy=aktY+ELM_SIZE+pDoc->m_Implikants[implikantIndex].count*ELM_OFFSET_VAR_SIZE;

			elmAND_Point=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);

			//napojenie clena
			tmpIndex=0;
			for(i=0;i<(int)pDoc->m_VariablesCount;i++)if(pDoc->m_Implikants[implikantIndex].binValues[i]!=BIT_UNDEFINED){
				if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ZERO){
					pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
				}
				if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ONE){
					pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
				}
				pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
				tmpIndex++;
			}
			if(pDoc->m_Implikants[implikantIndex].count==1){
				CPoint actPos=pDC->GetCurrentPosition();
				pDC->LineTo(actPos.x,actPos.y-ELM_OFFSET_VAR_SIZE);
				pDC->LineTo(actPos.x-OFFSET_VAR/2,actPos.y-ELM_OFFSET_VAR_SIZE);
				pDC->LineTo(actPos.x-OFFSET_VAR/2,actPos.y);
				//DrawConector(pDC,actPos.x-OFFSET_VAR/2,actPos.y);
				conectorsPoint[conectorsPointCount++]=CPoint(actPos.x-OFFSET_VAR/2,actPos.y);
			}

			//napojenie vystupu na NAND clen
			int tmpX,tmpY;
			tmpX=ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE;
			if(pDoc->m_Implikants[implikantIndex].count>1){
				tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2;
			}else{
				tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2;
			}
			pDC->MoveTo(tmpX,tmpY);
			pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,tmpY);
			pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
			pDC->LineTo(elmOR.x,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);

			aktY+=2*OFFSET_VAR+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE;
			implikantIndex++;
		}
		valuesCount++;
	}
	while(implikantIndex<pDoc->m_ImplikantsCount){
		DWORD tmpIndex;

		//vykreslenie clena AND
		if(pDoc->m_Implikants[implikantIndex].count>1){
			DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY,pDoc->m_Implikants[implikantIndex].count,'&',TRUE);
		}else{
			DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY,2,'&',TRUE);
		}
		
		if(aktY+ELM_SIZE*2+pDoc->m_Implikants[implikantIndex].count*ELM_OFFSET_VAR_SIZE>(DWORD)schemeSize.cy)schemeSize.cy=aktY+ELM_SIZE*2+pDoc->m_Implikants[implikantIndex].count*ELM_OFFSET_VAR_SIZE;
		elmAND_Point=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
		//napojenie clena
		tmpIndex=0;
		for(i=0;i<(int)pDoc->m_VariablesCount;i++)if(pDoc->m_Implikants[implikantIndex].binValues[i]!=BIT_UNDEFINED){
			if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ZERO){
				pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
			}
			if(pDoc->m_Implikants[implikantIndex].binValues[i]==BIT_ONE){
				pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
			}
			pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
			tmpIndex++;
		}
		if(pDoc->m_Implikants[implikantIndex].count==1){
			CPoint actPos=pDC->GetCurrentPosition();
			pDC->LineTo(actPos.x,actPos.y-ELM_OFFSET_VAR_SIZE);
			pDC->LineTo(actPos.x-OFFSET_VAR/2,actPos.y-ELM_OFFSET_VAR_SIZE);
			pDC->LineTo(actPos.x-OFFSET_VAR/2,actPos.y);
			//DrawConector(pDC,actPos.x-OFFSET_VAR/2,actPos.y);
			conectorsPoint[conectorsPointCount++]=CPoint(actPos.x-OFFSET_VAR/2,actPos.y);
		}

		//napojenie vystupu na NAND clen
		int tmpX,tmpY;
		tmpX=ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE;
		if(pDoc->m_Implikants[implikantIndex].count>1){
			tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE;
		}else{
			tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE;
		}
		pDC->MoveTo(tmpX,tmpY);
		pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,tmpY);
		pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(elmOR.x,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
		aktY+=2*OFFSET_VAR+(pDoc->m_Implikants[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE;
		implikantIndex++;
	}

	//je potrebne nakreslit aj NAND clen - posledna negacia
	if(pDoc->m_ImplikantsCount>1){
		DrawLogicalElement(pDC,elmOR.x,elmOR.y,pDoc->m_ImplikantsCount,'&',TRUE);
		pDC->MoveTo(elmOR.x+ELM_OFFSET_VAR_SIZE+OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCount-1)*ELM_OFFSET_VAR_SIZE)/2);
		pDC->LineTo(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCount-1)*ELM_OFFSET_VAR_SIZE)/2);
		strSize=pDC->GetTextExtent("y");
		pDC->TextOut(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR-strSize.cx,-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCount-1)*ELM_OFFSET_VAR_SIZE)/2+strSize.cy+ELM_OFFSET_VAR_SIZE,"y");
	}else{
		DrawLogicalElement(pDC,elmOR.x,elmOR.y,2,'&',TRUE);
		pDC->MoveTo(elmOR.x+ELM_OFFSET_VAR_SIZE+OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2);
		pDC->LineTo(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2);
		strSize=pDC->GetTextExtent("y");
		pDC->TextOut(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR-strSize.cx,-elmOR.y-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2+strSize.cy+ELM_OFFSET_VAR_SIZE,"y");
		pDC->MoveTo(elmOR.x,-elmOR.y-ELM_SIZE-ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(elmOR.x-OFFSET_VAR/2,-elmOR.y-ELM_SIZE-ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(elmOR.x-OFFSET_VAR/2,-elmOR.y-ELM_SIZE);
		//DrawConector(pDC,elmOR.x-OFFSET_VAR/2,-elmOR.y-ELM_SIZE);
		conectorsPoint[conectorsPointCount++]=CPoint(elmOR.x-OFFSET_VAR/2,-elmOR.y-ELM_SIZE);
	}

	schemeSize.cx=elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR-strSize.cx+OFFSET_VAR;

	schemeSize.cy-=ofsY;

	CPen bluePen,blackPen,*defPen;
	bluePen.CreatePen(PS_SOLID,0,RGB(0,0,255));
	blackPen.CreatePen(PS_SOLID,0,RGB(0,0,0));
	defPen=pDC->SelectObject(&blackPen);

	//nakreslenie zvyslych kontaktnych liniek
	for(k=0;k<(int)pDoc->m_VariablesCount;k++){
		pDC->SelectObject(&bluePen);
		pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-OFFSET_VAR);
		pDC->LineTo(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-schemeSize.cy);
		pDC->SelectObject(&blackPen);
		sprintf(str,"%c",pDoc->m_VariablesCount-k-1+'a');
		pDC->TextOut(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,str);
		if(useInvert[k]){
			//tuto premennu potrebujem aj v negovanom stave
			pDC->SelectObject(&bluePen);
			pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-OFFSET_VAR);
			pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-schemeSize.cy-ELM_SIZE);
			pDC->SelectObject(&blackPen);
			pDC->TextOut(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,str);
			strSize=pDC->GetTextExtent(str);
			pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2);
			pDC->LineTo(strSize.cx+ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2);
		}
	}

	for(k=0;k<conectorsPointCount;k++)DrawConector(pDC,conectorsPoint[k].x,conectorsPoint[k].y);
	pDC->SelectObject(defPen);

	schemeSize.cy+=OFFSET_VAR+m_FontHeight[2]+m_FontHeight[2]/2;
	pDC->SelectObject(defFont);

	return schemeSize;
}

CSize CLogObvodyView::Draw_NOR(CDC *pDC,int ofsX,int ofsY)
{
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);
	CLogObvodyDoc* pDoc = GetDocument();
	BOOL useInvert[32];	//ci danu premennu potrebujem aj negovanu a ci vobec potrebujem invertory
	CSize schemeSize(0,0);
	int k,i;
	CSize strSize;
	char str[256];
	CPoint conectorsPoint[100];	//jednotlive konektory
	int conectorsPointCount=0;	//pocet konektorov

	pDC->SelectObject(&m_Fonts[2]);
	pDC->TextOut(ofsX,-ofsY,"Kontaktná schéma v Pierceho algebre:");
	ofsY+=m_FontHeight[2]+m_FontHeight[2]/2;
	pDC->SelectObject(&m_Fonts[0]);

	for(k=0;k<32;k++)useInvert[k]=FALSE;
	for(k=0;k<(int)pDoc->m_ImplikantsCountInv;k++){
		for(i=(int)pDoc->m_VariablesCount-1;i>=0;i--){
			if(pDoc->m_ImplikantsInv[k].binValues[i]==BIT_ONE)useInvert[i]=TRUE;
		}
	}

	DWORD implikantIndex=0,valuesCount;

	//nakreslenie INVERT(NOR) a NOR clenov
	valuesCount=0;
	implikantIndex=0;
	int aktY=ofsY+OFFSET_VAR;
	POINT elmOR;	//ked je roh clena NOR
	elmOR.x=ofsX+(pDoc->m_VariablesCount-1)*OFFSET_VAR*2+OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR*2+ELM_OFFSET_VAR_SIZE*(pDoc->m_ImplikantsCountInv-1);
	elmOR.y=ofsY+OFFSET_VAR;
	CPoint elmAND_Point;	//ak je iba jeden AND clen -> toto je suradnica jeho vystupu
	for(k=0;k<(int)pDoc->m_VariablesCount;k++)if(useInvert[k]){
		//vykreslenie clena INVERT(NOR)
		DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR,aktY,2,'1',TRUE);
		if(aktY+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE>schemeSize.cy)schemeSize.cy=aktY+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE;
		//napojenie clena
		pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR,-aktY-ELM_SIZE);
		//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);
		conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE);

		//napojenie 2. vstupu
		pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR-OFFSET_VAR/2,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR-OFFSET_VAR/2,-aktY-ELM_SIZE);
		//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR-OFFSET_VAR/2,-aktY-ELM_SIZE);
		conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR-OFFSET_VAR/2,-aktY-ELM_SIZE);

		pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE/2);
		pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE/2);
		//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE/2);
		conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-aktY-ELM_SIZE-ELM_OFFSET_VAR_SIZE/2);

		aktY+=ELM_SIZE*2+OFFSET_VAR;
		if(implikantIndex<pDoc->m_ImplikantsCountInv){
			DWORD tmpIndex;

			//vykreslenie clena NOR
			if(pDoc->m_ImplikantsInv[implikantIndex].count>1){
				DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY-ELM_SIZE,pDoc->m_ImplikantsInv[implikantIndex].count,'1',TRUE);
			}else{
				DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY-ELM_SIZE,2,'1',TRUE);
			}
			if(aktY+ELM_SIZE+pDoc->m_ImplikantsInv[implikantIndex].count*ELM_OFFSET_VAR_SIZE>(DWORD)schemeSize.cy)schemeSize.cy=aktY+ELM_SIZE+pDoc->m_ImplikantsInv[implikantIndex].count*ELM_OFFSET_VAR_SIZE;
			elmAND_Point=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_ImplikantsInv[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2);
			//napojenie clena
			tmpIndex=0;
			for(i=0;i<(int)pDoc->m_VariablesCount;i++)if(pDoc->m_ImplikantsInv[implikantIndex].binValues[i]!=BIT_UNDEFINED){
				if(pDoc->m_ImplikantsInv[implikantIndex].binValues[i]==BIT_ONE){
					pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
				}
				if(pDoc->m_ImplikantsInv[implikantIndex].binValues[i]==BIT_ZERO){
					pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
					conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
				}
				pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE);
				tmpIndex++;
			}
			if(pDoc->m_ImplikantsInv[implikantIndex].count==1){
				CPoint actPos=pDC->GetCurrentPosition();
				pDC->LineTo(actPos.x,actPos.y-ELM_OFFSET_VAR_SIZE);
				pDC->LineTo(actPos.x-OFFSET_VAR/2,actPos.y-ELM_OFFSET_VAR_SIZE);
				pDC->LineTo(actPos.x-OFFSET_VAR/2,actPos.y);
				//DrawConector(pDC,actPos.x-OFFSET_VAR/2,actPos.y);
				conectorsPoint[conectorsPointCount++]=CPoint(actPos.x-OFFSET_VAR/2,actPos.y);
			}

			//napojenie vystupu na NOR clen
			int tmpX,tmpY;
			tmpX=ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE;
			if(pDoc->m_ImplikantsInv[implikantIndex].count>1){
				tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_ImplikantsInv[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2;
			}else{
				tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2;
			}
			//tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_ImplikantsInv[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2;
			pDC->MoveTo(tmpX,tmpY);
			pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,tmpY);
			pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
			pDC->LineTo(elmOR.x,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);

			aktY+=OFFSET_VAR+(pDoc->m_ImplikantsInv[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE;
			implikantIndex++;
		}
		valuesCount++;
	}
	
	while(implikantIndex<pDoc->m_ImplikantsCountInv){
		DWORD tmpIndex;		
		//vykreslenie clena NOR
		if(pDoc->m_ImplikantsInv[implikantIndex].count>1){
			DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY,pDoc->m_ImplikantsInv[implikantIndex].count,'1',TRUE);
		}else{
			DrawLogicalElement(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,aktY,2,'1',TRUE);
		}
		if(aktY+2*ELM_SIZE+pDoc->m_ImplikantsInv[implikantIndex].count*ELM_OFFSET_VAR_SIZE>(DWORD)schemeSize.cy)schemeSize.cy=aktY+2*ELM_SIZE+pDoc->m_ImplikantsInv[implikantIndex].count*ELM_OFFSET_VAR_SIZE;
		elmAND_Point=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE,-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_ImplikantsInv[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE);
		//napojenie clena
		tmpIndex=0;
		for(i=0;i<(int)pDoc->m_VariablesCount;i++)if(pDoc->m_ImplikantsInv[implikantIndex].binValues[i]!=BIT_UNDEFINED){
			if(pDoc->m_ImplikantsInv[implikantIndex].binValues[i]==BIT_ONE){
				pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				//DrawConector(pDC,ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				conectorsPoint[conectorsPointCount++]=CPoint(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
			}
			if(pDoc->m_ImplikantsInv[implikantIndex].binValues[i]==BIT_ZERO){
				pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				//DrawConector(pDC,ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
				conectorsPoint[conectorsPointCount++]=CPoint(ofsX+(pDoc->m_VariablesCount-i-1)*OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
			}
			pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR,-aktY-tmpIndex*ELM_OFFSET_VAR_SIZE-ELM_SIZE);
			tmpIndex++;
		}

		//napojenie vystupu na NOR clen
		int tmpX,tmpY;
		tmpX=ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE;
		tmpX=ofsX+pDoc->m_VariablesCount*OFFSET_VAR*2+ELM_SIZE*2+OFFSET_VAR+ELM_SIZE*2+ELM_OFFSET_VAR_SIZE;
		if(pDoc->m_ImplikantsInv[implikantIndex].count>1){
			tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_ImplikantsInv[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE;
		}else{
			tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE;
		}
		//tmpY=-aktY+ELM_SIZE-(ELM_SIZE*2+(pDoc->m_ImplikantsInv[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE)/2-ELM_SIZE;
		pDC->MoveTo(tmpX,tmpY);
		pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,tmpY);
		pDC->LineTo(tmpX+OFFSET_VAR+implikantIndex*ELM_OFFSET_VAR_SIZE,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(elmOR.x,-elmOR.y-ELM_SIZE-implikantIndex*ELM_OFFSET_VAR_SIZE);
		aktY+=2*OFFSET_VAR+(pDoc->m_ImplikantsInv[implikantIndex].count-1)*ELM_OFFSET_VAR_SIZE;
		implikantIndex++;
	}

	//je potrebne nakreslit aj NOR clen
	if(pDoc->m_ImplikantsCountInv>1){
		DrawLogicalElement(pDC,elmOR.x,elmOR.y,pDoc->m_ImplikantsCountInv,'1',TRUE);
		pDC->MoveTo(elmOR.x+ELM_OFFSET_VAR_SIZE+OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCountInv-1)*ELM_OFFSET_VAR_SIZE)/2);
		pDC->LineTo(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCountInv-1)*ELM_OFFSET_VAR_SIZE)/2);
		strSize=pDC->GetTextExtent("y");
		pDC->TextOut(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR-strSize.cx,-elmOR.y-(ELM_SIZE*2+(pDoc->m_ImplikantsCountInv-1)*ELM_OFFSET_VAR_SIZE)/2+strSize.cy+ELM_OFFSET_VAR_SIZE,"y");
	}else{
		DrawLogicalElement(pDC,elmOR.x,elmOR.y,2,'1',TRUE);
		pDC->MoveTo(elmOR.x+ELM_OFFSET_VAR_SIZE+OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2);
		pDC->LineTo(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR,-elmOR.y-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2);
		strSize=pDC->GetTextExtent("y");
		pDC->TextOut(elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR-strSize.cx,-elmOR.y-(ELM_SIZE*2+ELM_OFFSET_VAR_SIZE)/2+strSize.cy+ELM_OFFSET_VAR_SIZE,"y");

		pDC->MoveTo(elmOR.x,-elmOR.y-ELM_SIZE-ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(elmOR.x-OFFSET_VAR/2,-elmOR.y-ELM_SIZE-ELM_OFFSET_VAR_SIZE);
		pDC->LineTo(elmOR.x-OFFSET_VAR/2,-elmOR.y-ELM_SIZE);
		//DrawConector(pDC,elmOR.x-OFFSET_VAR/2,-elmOR.y-ELM_SIZE);
		conectorsPoint[conectorsPointCount++]=CPoint(elmOR.x-OFFSET_VAR/2,-elmOR.y-ELM_SIZE);
	}
	schemeSize.cx=elmOR.x+ELM_OFFSET_VAR_SIZE+2*OFFSET_VAR-strSize.cx-ofsX;

	schemeSize.cy-=ofsY;

	CPen bluePen,blackPen,*defPen;
	bluePen.CreatePen(PS_SOLID,0,RGB(0,0,255));
	blackPen.CreatePen(PS_SOLID,0,RGB(0,0,0));
	defPen=pDC->SelectObject(&blackPen);

	//nakreslenie zvyslych kontaktnych liniek
	for(k=0;k<(int)pDoc->m_VariablesCount;k++){
		pDC->SelectObject(&bluePen);
		pDC->MoveTo(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-OFFSET_VAR);
		pDC->LineTo(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-schemeSize.cy-ELM_SIZE);
		pDC->SelectObject(&blackPen);
		sprintf(str,"%c",pDoc->m_VariablesCount-k-1+'a');
		pDC->TextOut(ofsX+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,str);
		if(useInvert[k]){
			//tuto premennu potrebujem aj v negovanom stave
			pDC->SelectObject(&bluePen);
			pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-OFFSET_VAR);
			pDC->LineTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR,-ofsY-schemeSize.cy-ELM_SIZE);
			pDC->SelectObject(&blackPen);
			pDC->TextOut(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,str);
			strSize=pDC->GetTextExtent(str);
			pDC->MoveTo(ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2);
			pDC->LineTo(strSize.cx+ofsX+pDoc->m_VariablesCount*OFFSET_VAR+ELM_SIZE*2+OFFSET_VAR+(pDoc->m_VariablesCount-k-1)*OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2,-ofsY-OFFSET_VAR+ELM_OFFSET_VAR_SIZE/2);
		}
	}

	for(k=0;k<conectorsPointCount;k++)DrawConector(pDC,conectorsPoint[k].x,conectorsPoint[k].y);
	pDC->SelectObject(defPen);

	schemeSize.cy+=OFFSET_VAR+m_FontHeight[2]+m_FontHeight[2]/2;
	pDC->SelectObject(defFont);

	return schemeSize;
}

CSize CLogObvodyView::DrawFunctionNOR(CDC *pDC,int ofsX,int ofsY)
{
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);
	CLogObvodyDoc* pDoc = GetDocument();
	CSize schemeSize(0,0),itemSize,strSize;
	int actPositionX=ofsX;
	int actPositionY=ofsY;

	//vypis funkcie v Pierceho algebre
	char str[256];
	int zalY=-actPositionY,k;
	pDC->SelectObject(&m_Fonts[2]);
	strcpy(str,"Funkcia zapísaná v Pierceho algebre (NOR):");
	strSize=pDC->GetTextExtent(str);
	TextOut(pDC,str,&actPositionX,&zalY);
	actPositionX=ofsX;
	actPositionY+=strSize.cy+OFS_INVERT_LINE*3;
	pDC->SelectObject(&m_Fonts[1]);

	strcpy(str,"y = ");
	zalY=-actPositionY;
	TextOut(pDC,str,&actPositionX,&zalY);
	pDC->SelectObject(&m_Fonts[0]);

	CPoint startPoint(actPositionX,actPositionY);
	for(k=0;k<(int)pDoc->m_ImplikantsCountInv;k++){
		itemSize=DrawImplicent(pDC,actPositionX,-actPositionY,&pDoc->m_ImplikantsInv[k]);
		pDC->MoveTo(actPositionX,-actPositionY+OFS_INVERT_LINE);
		actPositionX+=itemSize.cx;
		actPositionY+=itemSize.cy;
		pDC->LineTo(actPositionX,-actPositionY+OFS_INVERT_LINE);

		if(k<(int)pDoc->m_ImplikantsCountInv-1){
			zalY=-actPositionY;
			TextOut(pDC," + ",&actPositionX,&zalY);
		}
	}
	pDC->MoveTo(startPoint.x,-startPoint.y+OFS_INVERT_LINE*2);
	pDC->LineTo(actPositionX,-actPositionY+OFS_INVERT_LINE*2);
	if(pDoc->m_ImplikantsCountInv==0){
		sprintf(str,"%d",pDoc->m_FunctionFlagInv);
		zalY=-actPositionY;
		TextOut(pDC,str,&actPositionX,&zalY);
	}

	schemeSize.cx=actPositionX-ofsX;
	schemeSize.cy=actPositionY-ofsY+OFS_INVERT_LINE*3;
	pDC->SelectObject(defFont);
	return schemeSize;
}

CSize CLogObvodyView::DrawFunctionAND_OR_INVERT(CDC *pDC,int ofsX,int ofsY)
{
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);
	CLogObvodyDoc* pDoc = GetDocument();
	CSize schemeSize(0,0),itemSize,strSize;
	int actPositionX=ofsX;
	int actPositionY=ofsY;
	char str[256];
	int k;

	pDC->SelectObject(&m_Fonts[2]);
	pDC->TextOut(actPositionX,-actPositionY,"Funkcia zapísaná v Booleovej algebre (AND-OR-INVERT):");
	actPositionY+=m_FontHeight[2]+m_FontHeight[2]/2;
	pDC->SelectObject(&m_Fonts[1]);
	strcpy(str,"y = ");
	int zalY=-actPositionY;
	TextOut(pDC,str,&actPositionX,&zalY);
	zalY+=m_FontHeight[1];
	pDC->SelectObject(&m_Fonts[0]);

	for(k=0;k<(int)pDoc->m_ImplikantsCount;k++){
		itemSize=DrawImplikant(pDC,actPositionX,-actPositionY,&pDoc->m_Implikants[k]);
		actPositionX+=itemSize.cx;
		actPositionY+=itemSize.cy;

		if(k<(int)pDoc->m_ImplikantsCount-1){
			zalY=-actPositionY;
			TextOut(pDC," + ",&actPositionX,&zalY);
		}
	}
	if(pDoc->m_ImplikantsCount==0){
		sprintf(str,"%d",pDoc->m_FunctionFlag);
		zalY=-actPositionY;
		TextOut(pDC,str,&actPositionX,&zalY);
	}

	schemeSize.cx=actPositionX-ofsX;
	schemeSize.cy=actPositionY-ofsY+m_FontHeight[0];
	pDC->SelectObject(defFont);
	return schemeSize;
}

CSize CLogObvodyView::DrawFunctionNAND(CDC *pDC,int ofsX,int ofsY)
{
	CFont *defFont=pDC->SelectObject(&m_Fonts[0]);
	CLogObvodyDoc* pDoc = GetDocument();
	CSize schemeSize(0,0),itemSize,strSize;
	int actPositionX=ofsX;
	int actPositionY=ofsY;

	//vypis funkcie v Shefferovej algebre
	char str[256];
	int zalY=-actPositionY,k;
	pDC->SelectObject(&m_Fonts[2]);
	strcpy(str,"Funkcia zapísaná v Shefferovej algebre (NAND):");
	strSize=pDC->GetTextExtent(str);
	TextOut(pDC,str,&actPositionX,&zalY);
	actPositionX=ofsX;
	actPositionY+=strSize.cy+OFS_INVERT_LINE*3;
	pDC->SelectObject(&m_Fonts[1]);
	strcpy(str,"y = ");
	zalY=-actPositionY;
	TextOut(pDC,str,&actPositionX,&zalY);
	pDC->SelectObject(&m_Fonts[0]);

	CPoint startPoint(actPositionX,actPositionY);
	for(k=0;k<(int)pDoc->m_ImplikantsCount;k++){
		itemSize=DrawImplikant(pDC,actPositionX,-actPositionY,&pDoc->m_Implikants[k]);
		pDC->MoveTo(actPositionX,-actPositionY+OFS_INVERT_LINE);
		actPositionX+=itemSize.cx;
		actPositionY+=itemSize.cy;
		pDC->LineTo(actPositionX,-actPositionY+OFS_INVERT_LINE);

		if(k<(int)pDoc->m_ImplikantsCount-1){
			zalY=-actPositionY;
			TextOut(pDC," . ",&actPositionX,&zalY);
		}
	}
	pDC->MoveTo(startPoint.x,-startPoint.y+OFS_INVERT_LINE*2);
	pDC->LineTo(actPositionX,-actPositionY+OFS_INVERT_LINE*2);
	if(pDoc->m_ImplikantsCount==0){
		sprintf(str,"%d",pDoc->m_FunctionFlag);
		zalY=-actPositionY;
		TextOut(pDC,str,&actPositionX,&zalY);
	}

	schemeSize.cx=actPositionX-ofsX;
	schemeSize.cy=actPositionY-ofsY+OFS_INVERT_LINE*3;
	pDC->SelectObject(defFont);
	return schemeSize;
}

void CLogObvodyView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_LOMETRIC, sizeTotal);
}

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyView printing

BOOL CLogObvodyView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(5);
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLogObvodyView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	/*
	if(pDC->IsPrinting()){
		CRect rectClient;
		GetClientRect(&rectClient);
		
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(1000,1000);
		//pDC->SetViewportExt(pInfo->m_rectDraw.right,pInfo->m_rectDraw.bottom);
		//pDC->SetViewportOrg(0,0);
		pDC->SetViewportExt(rectClient.right,-rectClient.bottom);
		pDC->SetViewportExt(rectClient.right/2,rectClient.bottom/2);
		
	}
	*/
	CScrollView::OnPrepareDC(pDC, pInfo);
}

void CLogObvodyView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLogObvodyView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CLogObvodyView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CLogObvodyDoc* pDoc = GetDocument();

	//pDC->SetMapMode(MM_ISOTROPIC);
	//CSize prev=pDC->SetViewportExt(10000,-10000);
	
	// TODO: Add your specialized code here and/or call the base class	
	//pDC->SetMapMode(MM_LOMETRIC);
	CSize sizePrint=pDC->GetWindowExt(),itemSize,itemSize2;
	sizePrint.cx-=PRINT_BORDER*2;
	sizePrint.cy-=PRINT_BORDER*2;
	if(pDoc->m_BoolFunctionValues!=NULL && pInfo->m_nCurPage==1){
		DrawTableOfFunctionValues(pDC,PRINT_BORDER+(sizePrint.cx-m_SizeTableFunctionValues.cx)/2,PRINT_BORDER+(sizePrint.cy-m_SizeTableFunctionValues.cy)/2);
	}
	if(pDoc->m_ImplikantsCount>0){
		if(pDoc->m_BoolFunctionValues!=NULL && pInfo->m_nCurPage==2){
			itemSize=DrawCalculateSteps(pDC,PRINT_BORDER,PRINT_BORDER);
			DrawTableMPI(pDC,PRINT_BORDER,PRINT_BORDER+itemSize.cy+m_FontHeight[0]);
		}

		if(pDoc->m_BoolFunctionValues!=NULL && pInfo->m_nCurPage==3){
			itemSize=DrawFunctionAND_OR_INVERT(pDC,PRINT_BORDER,PRINT_BORDER);
			Draw_AND_OR_INVERT(pDC,PRINT_BORDER,PRINT_BORDER+itemSize.cy+m_FontHeight[0]);
		}

		if(pDoc->m_BoolFunctionValues!=NULL && pInfo->m_nCurPage==4){
			itemSize=DrawFunctionNAND(pDC,PRINT_BORDER,PRINT_BORDER);
			Draw_NAND(pDC,PRINT_BORDER,PRINT_BORDER+itemSize.cy+m_FontHeight[0]);
		}
		if(pDoc->m_BoolFunctionValues!=NULL && pInfo->m_nCurPage==5){
			itemSize=DrawFunctionNOR(pDC,PRINT_BORDER,PRINT_BORDER);
			Draw_NOR(pDC,PRINT_BORDER,PRINT_BORDER+itemSize.cy+m_FontHeight[0]);
		}
	}
	
	//CScrollView::OnPrint(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyView diagnostics

#ifdef _DEBUG
void CLogObvodyView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CLogObvodyView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CLogObvodyDoc* CLogObvodyView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLogObvodyDoc)));
	return (CLogObvodyDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyView message handlers

void CLogObvodyView::OnTableChange() 
{
	CLogObvodyDoc* pDoc = GetDocument();
	CTableOfBoolFunctionDlg dlg;
	dlg.m_InitBoolFunction=pDoc->m_BoolFunctionValues;
	dlg.m_InitVariablesCount=pDoc->m_VariablesCount;

	if(dlg.DoModal()==IDOK){
		pDoc->SetTitle("Untitled");
		//zmenena funkcia
		pDoc->Kill();
		pDoc->m_BoolFunctionValues=new BYTE[dlg.m_ItemsCount];
		if(pDoc->m_BoolFunctionValues==NULL){
			pDoc->m_VariablesCount=0;
			pDoc->m_ValuesCount=0;
			MessageBox("Nedostatok pamate na zadanie novej funkcie","Error",MB_OK|MB_ICONERROR);
			return;
		}
		pDoc->m_VariablesCount=dlg.m_VariablesCount;
		pDoc->m_ValuesCount=dlg.m_ItemsCount;

		DWORD k,i;
		for(k=0;k<pDoc->m_ValuesCount;k++)pDoc->m_BoolFunctionValues[k]=dlg.m_BoolFunction[k];
		if(m_QuinMcCluskey->SetFunction(pDoc->m_BoolFunctionValues,pDoc->m_VariablesCount)==-1){
			MessageBox("Nedostatok pamate na vypocet minimalizacie funkcie","Error",MB_OK|MB_ICONERROR);
			return;
		}
		SImplikant *implikants=new SImplikant[pDoc->m_ValuesCount]; //jednotlive implikanty
		DWORD implikantsCount;
		if(implikants==NULL){
			MessageBox("Nedostatok pamate na vypocet minimalizacie funkcie","Error",MB_OK|MB_ICONERROR);
			return;
		}
		//Vypocet DNF
		m_Calculating=TRUE;
		CStatusDlg *dlgStatus=new CStatusDlg(this);
		dlgStatus->Create();
		//if(dlgStatus->GetSafeHwnd()==NULL)dlgStatus->ShowWindow(TRUE);
		dlgStatus->ShowWindow(TRUE);
		dlgStatus->GetDlgItem(IDC_STATIC_TEXT1)->SetWindowText("Vypocet v Booleovej algebre");
		m_QuinMcCluskey->SetStatusDialog(dlgStatus);
		m_QuinMcCluskey->m_Status=STATUS_NONE;
		dlgStatus->SetCalculatingClass(m_QuinMcCluskey);

		m_QuinMcCluskey->SetDocument(pDoc);
		m_QuinMcCluskey->MinimalizeFunction(implikants,&implikantsCount);
		if(m_QuinMcCluskey->m_Status==STATUS_FINISH){
			for(k=0;k<implikantsCount;k++){
				implikants[k].count=0;
				for(i=0;i<pDoc->m_VariablesCount;i++)if(implikants[k].binValues[i]!=BIT_UNDEFINED)implikants[k].count++;
			}
			pDoc->m_ImplikantsCount=implikantsCount;
			pDoc->m_Implikants=new SImplikant[implikantsCount];
			for(k=0;k<implikantsCount;k++)pDoc->m_Implikants[k]=implikants[k];
			pDoc->m_FunctionFlag=implikants[0].flag;
			//SAFE_DELETE(implikants);
		}else{
			pDoc->Kill();
			m_Calculating=FALSE;
			Invalidate();
			SAFE_DELETE(dlgStatus);
			SAFE_DELETE(implikants);
			MessageBox("Vypocet preruseny","Warning",MB_OK|MB_ICONWARNING);
			return;
		}
		m_Calculating=FALSE;

		//Vypocet DNF pre negovanu fciu -> vypocet KNF
		m_QuinMcCluskey->SetDocument(NULL);
		m_QuinMcCluskey->m_Status=STATUS_NONE;
		for(k=0;k<pDoc->m_ValuesCount;k++){
			if(dlg.m_BoolFunction[k]==BIT_ONE) pDoc->m_BoolFunctionValues[k]=BIT_ZERO;else
			if(dlg.m_BoolFunction[k]==BIT_ZERO)pDoc->m_BoolFunctionValues[k]=BIT_ONE;else
			                                   pDoc->m_BoolFunctionValues[k]=BIT_UNDEFINED;
		}
		if(m_QuinMcCluskey->SetFunction(pDoc->m_BoolFunctionValues,pDoc->m_VariablesCount)==-1){
			MessageBox("Nedostatok pamate na vypocet minimalizacie funkcie","Error",MB_OK|MB_ICONERROR);
			SAFE_DELETE(dlgStatus);
			return;
		}
		dlgStatus->ShowWindow(TRUE);
		dlgStatus->GetDlgItem(IDC_STATIC_TEXT1)->SetWindowText("Vypocet v Piercovej algebre");
		m_Calculating=TRUE;
		m_QuinMcCluskey->MinimalizeFunction(implikants,&implikantsCount);
		if(m_QuinMcCluskey->m_Status==STATUS_FINISH){
			for(k=0;k<implikantsCount;k++){
				implikants[k].count=0;
				for(i=0;i<pDoc->m_VariablesCount;i++)if(implikants[k].binValues[i]!=BIT_UNDEFINED)implikants[k].count++;
			}
			pDoc->m_ImplikantsCountInv=implikantsCount;
			pDoc->m_ImplikantsInv=new SImplikant[implikantsCount];
			for(k=0;k<implikantsCount;k++)pDoc->m_ImplikantsInv[k]=implikants[k];
			pDoc->m_FunctionFlagInv=implikants[0].flag;
		}else{
			pDoc->Kill();
			m_Calculating=FALSE;
			Invalidate();
			SAFE_DELETE(dlgStatus);
			SAFE_DELETE(implikants);
			MessageBox("Vypocet preruseny","Warning",MB_OK|MB_ICONWARNING);
			return;
		}
		for(k=0;k<pDoc->m_ValuesCount;k++)pDoc->m_BoolFunctionValues[k]=dlg.m_BoolFunction[k];
		SAFE_DELETE(implikants);

		Invalidate();
		SAFE_DELETE(dlgStatus);
		m_Calculating=FALSE;
	}
}

void CLogObvodyView::OnViewSettings() 
{
	COutputView dlg;
	//dlg.m_ViewHelp=m_ViewHelp;
	dlg.m_ViewTableFunctionValues=m_ViewTableFunctionValues;
	dlg.m_ViewCalculateSteps=m_ViewCalculateSteps;
	dlg.m_ViewTableMPI=m_ViewTableMPI;
	dlg.m_View_AND_OR_INVERT_Scheme=m_View_AND_OR_INVERT_Scheme;
	dlg.m_View_NAND_Scheme=m_View_NAND_Scheme;
	dlg.m_View_NOR_Scheme=m_View_NOR_Scheme;
	dlg.m_ViewFunctionBool=m_ViewFunctionBool;
	dlg.m_ViewFunctionSheffer=m_ViewFunctionSheffer;
	dlg.m_ViewFunctionPierce=m_ViewFunctionPierce;

	if(dlg.DoModal()==IDOK){
		//m_ViewHelp=dlg.m_ViewHelp;
		m_ViewTableFunctionValues=dlg.m_ViewTableFunctionValues;
		m_ViewCalculateSteps=dlg.m_ViewCalculateSteps;
		m_ViewTableMPI=dlg.m_ViewTableMPI;
		m_View_AND_OR_INVERT_Scheme=dlg.m_View_AND_OR_INVERT_Scheme;
		m_View_NAND_Scheme=dlg.m_View_NAND_Scheme;
		m_View_NOR_Scheme=dlg.m_View_NOR_Scheme;
		m_ViewFunctionBool=dlg.m_ViewFunctionBool;
		m_ViewFunctionSheffer=dlg.m_ViewFunctionSheffer;
		m_ViewFunctionPierce=dlg.m_ViewFunctionPierce;
		Invalidate();
	}
}

void CLogObvodyView::OnFilePrintPreview() 
{
	// In derived classes, implement special window handling here
	// Be sure to Unhook Frame Window close if hooked.
    
	// must not create this on the frame.  Must outlive this function
	CPrintPreviewState* pState = new CPrintPreviewState;
    
	// DoPrintPreview's return value does not necessarily indicate that
	// Print preview succeeded or failed, but rather what actions are necessary
	// at this point.  If DoPrintPreview returns TRUE, it means that
	// OnEndPrintPreview will be (or has already been) called and the
	// pState structure will be/has been deleted.
	// If DoPrintPreview returns FALSE, it means that OnEndPrintPreview
	// WILL NOT be called and that cleanup, including deleting pState
	// must be done here.
    
	if (!DoPrintPreview(IDD_PREVIEW, this, RUNTIME_CLASS(CMultiPagePreviewView),
						pState)) // note, put your class name in here
	{
		// In derived classes, reverse special window handling here for
		// Preview failure case
        
		TRACE0("Error: DoPrintPreview failed.\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		delete pState;      // preview failed to initialize, delete State now
		pState = NULL;
	}	
}

void CLogObvodyView::OnFileSave() 
{
	CLogObvodyDoc* pDoc = GetDocument();
	CFileDialog dlg(FALSE);
	if(dlg.DoModal()==IDOK){
		FILE *f=fopen(dlg.GetPathName(),"wt");
		if(f==NULL){
			MessageBox("Nedostatok miesta na disku","Error",MB_OK|MB_ICONERROR);
			return;
		}
		DWORD k;
		pDoc->SetTitle(dlg.GetPathName());

		fprintf(f,"VariablesCount %d\n",pDoc->m_VariablesCount);
		for(k=0;k<pDoc->m_ValuesCount;k++){
			char charValue='x';
			if(pDoc->m_BoolFunctionValues[k]==BIT_ONE)charValue='1';
			if(pDoc->m_BoolFunctionValues[k]==BIT_ZERO)charValue='0';
			fprintf(f,"FunctionValueDE %d %c\n",k,charValue);
		}
		fclose(f);
	}	
}

void CLogObvodyView::OnHelpProgram() 
{
	CProgramHelpDlg dlg;
	dlg.DoModal();
}
