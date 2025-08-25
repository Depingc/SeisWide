#pragma once

// Generic template
template <typename R, typename ...Args>
class WC
{
private:

	template <typename F, typename Tuple, bool Done, int Total, int... N>
	struct call_impl
	{
		static R callthefunction(F f, Tuple && t)
		{
			return (R)call_impl<F, Tuple, Total == 1 + sizeof...(N),
				Total, N..., sizeof...(N)>::callthefunction(f, std::forward<Tuple>(t));
		}
	};

	template <typename F, typename Tuple, int Total, int... N>
	struct call_impl<F, Tuple, true, Total, N...>
	{
		static R callthefunction(F f, Tuple && t)
		{
			UNREFERENCED_PARAMETER(t);
			return (R)f(std::get<N>(std::forward<Tuple>(t))...);
		}
	};

	template <typename F, typename Tuple>
	static R callthefunction(F f, Tuple && t)
	{
		typedef typename std::decay<Tuple>::type ttype;
		return (R)call_impl<F, Tuple, 0 == std::tuple_size<ttype>::value,
			std::tuple_size<ttype>::value>::callthefunction(f, std::forward<Tuple>(t));
	}

	struct WAITCALL
	{
		wstring txt;
		std::function<R(Args...)> f;
		std::tuple<Args...> tu;
		R rval;
	};

	static void WaitCallDPCall(HWND hh, WAITCALL* w)
	{
		w->rval = callthefunction(w->f, w->tu);
		SendMessage(hh, WM_USER + 1, 0, 0);
		return;
	}

	static INT_PTR __stdcall WaitCallDP(HWND hh, UINT mm, WPARAM, LPARAM ll)
	{
		WAITCALL* eel = (WAITCALL*)GetWindowLongPtr(hh, GWLP_USERDATA);
		switch (mm)
		{
		case WM_USER + 1:
		{
			EndDialog(hh, IDOK);
			return 0;
		}
		case WM_INITDIALOG:
		{
			eel = (WAITCALL*)ll;
			SetWindowLongPtr(hh, GWLP_USERDATA, (LONG_PTR)eel);
			SetDlgItemText(hh, 701, eel->txt.c_str());
			SendDlgItemMessage(hh, 901, PBM_SETMARQUEE, true, 0);
			std::thread t(WaitCallDPCall, hh, eel);
			t.detach();
			return 0;
		}
		}
		return 0;
	}

public:

	// User calls this
	static R WaitCall(HWND hh, const wchar_t* txt, std::function<R(Args...)> fn, Args... a)
	{
		WAITCALL w;
		w.txt = txt;
		w.f = fn;
		w.tu = ::make_tuple<Args...>(std::forward<Args>(a)...);
		const char* res = "\x01\x00\xFF\xFF\x00\x00\x00\x00\x80\x00\x00\x00\
						              x48\x08\x00\x80\x03\x00\x00\x00\x00\x00\x35\x01\x70\x00\x00\x00\x00\x00\
									              x00\x00\x08\x00\x90\x01\x00\x01\x4D\x00\x53\x00\x20\x00\x53\x00\x68\x00\
												              x65\x00\x6C\x00\x6C\x00\x20\x00\x44\x00\x6C\x00\x67\x00\x00\x00\x00\x00\
															              x00\x00\x00\x00\x00\x00\x07\x00\x00\x50\x07\x00\x07\x00\x27\x01\x62\x00\
																		              xFF\xFF\xFF\xFF\xFF\xFF\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
																					              x00\x00\x01\x02\x02\x50\x1F\x00\x15\x00\xF6\x00\x30\x00\xBD\x02\x00\x00\
																								              xFF\xFF\x82\x00\x53\x00\x74\x00\x61\x00\x74\x00\x69\x00\x63\x00\x00\x00\
																											              x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x09\x00\x80\x50\x1F\x00\x50\x00\
																														              xF6\x00\x0E\x00\x85\x03\x00\x00\x6D\x00\x73\x00\x63\x00\x74\x00\x6C\x00\
																																	              x73\x00\x5F\x00\x70\x00\x72\x00\x6F\x00\x67\x00\x72\x00\x65\x00\x73\x00\
																																				              x73\x00\x33\x00\x32\x00\x00\x00\x00\x00\x00\x00";
		DialogBoxIndirectParam(GetModuleHandle(0), (LPCDLGTEMPLATEW)res, hh, WaitCallDP, (LPARAM)&w);
		return (R)w.rval;
	}
};







// Partially specialized for R = void
template <typename ...Args>
class WC<void, Args...>
{
private:

	template <typename F, typename Tuple, bool Done, int Total, int... N>
	struct call_impl
	{
		static void callthefunction(F f, Tuple && t)
		{
			call_impl<F, Tuple, Total == 1 + sizeof...(N), Total, N...,
				sizeof...(N)>::callthefunction(f, std::forward<Tuple>(t));
		}
	};

	template <typename F, typename Tuple, int Total, int... N>
	struct call_impl<F, Tuple, true, Total, N...>
	{
		static void callthefunction(F f, Tuple && t)
		{
			UNREFERENCED_PARAMETER(t);
			f(std::get<N>(std::forward<Tuple>(t))...);
		}
	};

	template <typename F, typename Tuple>
	static void callthefunction(F f, Tuple && t)
	{
		typedef typename std::decay<Tuple>::type ttype;
		call_impl<F, Tuple, 0 == std::tuple_size<ttype>::value,
			std::tuple_size<ttype>::value>::callthefunction(f, std::forward<Tuple>(t));
	}

	struct WAITCALL
	{
		wstring txt;
		std::function<void(Args...)> f;
		std::tuple<Args...> tu;
	};

	static void WaitCallDPCall(HWND hh, WAITCALL* w)
	{
		callthefunction(w->f, w->tu);
		SendMessage(hh, WM_USER + 1, 0, 0);
		return;
	}

	static INT_PTR __stdcall WaitCallDP(HWND hh, UINT mm, WPARAM, LPARAM ll)
	{
		WAITCALL* eel = (WAITCALL*)GetWindowLongPtr(hh, GWLP_USERDATA);
		switch (mm)
		{
		case WM_USER + 1:
		{
			EndDialog(hh, IDOK);
			return 0;
		}
		case WM_INITDIALOG:
		{
			eel = (WAITCALL*)ll;
			SetWindowLongPtr(hh, GWLP_USERDATA, (LONG_PTR)eel);
			SetDlgItemText(hh, 701, eel->txt.c_str());
			SendDlgItemMessage(hh, 901, PBM_SETMARQUEE, true, 0);
			std::thread t(WaitCallDPCall, hh, eel);
			t.detach();
			return 0;
		}
		}
		return 0;
	}

public:

	// User calls this
	static void WaitCall(HWND hh, const wchar_t* txt, std::function<void(Args...)> fn, Args... a)
	{
		WAITCALL w;
		w.txt = txt;
		w.f = fn;
		w.tu = ::make_tuple<Args...>(std::forward<Args>(a)...);
		const char* res = "\x01\x00\xFF\xFF\x00\x00\x00\x00\x80\x00\x00\
						              x00\x48\x08\x00\x80\x03\x00\x00\x00\x00\x00\x35\x01\x70\x00\x00\x00\
									              x00\x00\x00\x00\x08\x00\x90\x01\x00\x01\x4D\x00\x53\x00\x20\x00\x53\
												              x00\x68\x00\x65\x00\x6C\x00\x6C\x00\x20\x00\x44\x00\x6C\x00\x67\x00\
															              x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07\x00\x00\x50\x07\x00\x07\
																		              x00\x27\x01\x62\x00\xFF\xFF\xFF\xFF\xFF\xFF\x80\x00\x00\x00\x00\x00\
																					              x00\x00\x00\x00\x00\x00\x00\x00\x01\x02\x02\x50\x1F\x00\x15\x00\xF6\
																								              x00\x30\x00\xBD\x02\x00\x00\xFF\xFF\x82\x00\x53\x00\x74\x00\x61\x00\
																											              x74\x00\x69\x00\x63\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\
																														              x00\x09\x00\x80\x50\x1F\x00\x50\x00\xF6\x00\x0E\x00\x85\x03\x00\x00\
																																	              x6D\x00\x73\x00\x63\x00\x74\x00\x6C\x00\x73\x00\x5F\x00\x70\x00\x72\
																																				              x00\x6F\x00\x67\x00\x72\x00\x65\x00\x73\x00\x73\x00\x33\x00\x32\x00\
																																							              x00\x00\x00\x00\x00\x00";
		DialogBoxIndirectParam(GetModuleHandle(0), (LPCDLGTEMPLATEW)res, hh, WaitCallDP, (LPARAM)&w);
	}
};
