void CSeisWideDoc::OnEditPaste() 
{
	// TODO: Add your command handler code here
	
		BeginWaitCursor();

		HBITMAP hBmp = NULL;
		hBmp = (HBITMAP) GetClipboardData(CF_BITMAP);
		CloseClipboard();

		CBitmap m_bmp;
		m_bmp.FromHandle(hBmp);
		if(m_bmp.m_hObject == NULL)
		{
			EndWaitCursor();
		}

		CWnd *pWnd;
		pWnd = pWnd->GetActiveWindow( );
		CClientDC	dc(pWnd);
		CRect		rect;
		pWnd->GetClientRect(rect);

		bmp24.bmpDC.SelectObject(&m_bmp);
		bmp24.bmpDC.SetMapMode(MM_ANISOTROPIC);
		bmp24.bmpDC.SetViewportExt(rect.Width(), rect.Height());
		bmp24.bmpDC.SetWindowExt(rect.Width(), rect.Height());	 //for rect, logical, himetric coords.
		bmp24.bmpDC.SetBkMode(TRANSPARENT); //to avoid texts overlap each other.

		DrawBitmap(&dc);

		EndWaitCursor();
//	draw.sameDisplay = FALSE; //indicate display has been changed and will re-load all data.
	SetModifiedFlag();
	UpdateAllViews(NULL);
	NotifyChanged();
}

