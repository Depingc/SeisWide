// SrvrItem.h : interface of the CSeisWideSrvrItem class
//

class CSeisWideSrvrItem : public COleServerItem
{
	DECLARE_DYNAMIC(CSeisWideSrvrItem)

// Constructors
public:
	CSeisWideSrvrItem(CSeisWideDoc* pContainerDoc);

// Attributes
	CSeisWideDoc* GetDocument() const
		{ return (CSeisWideDoc*)COleServerItem::GetDocument(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeisWideSrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CSeisWideSrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////
