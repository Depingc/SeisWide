// CntrItem.h : interface of the CSeisWideCntrItem class
//

class CSeisWideDoc;
class CSeisWideView;

class CSeisWideCntrItem : public COleClientItem
{
	DECLARE_SERIAL(CSeisWideCntrItem)

// Constructors
public:
	CSeisWideCntrItem(CSeisWideDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CSeisWideDoc* GetDocument()
		{ return (CSeisWideDoc*)COleClientItem::GetDocument(); }
	CSeisWideView* GetActiveView()
		{ return (CSeisWideView*)COleClientItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeisWideCntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL CanActivate();
	//}}AFX_VIRTUAL

// Implementation
public:
	~CSeisWideCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////
