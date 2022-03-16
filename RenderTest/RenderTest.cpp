#define NOMINMAX
#define VC_EXTRALEAN
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_WINDOWS 0x0410
#define _WIN32_IE 0x0600
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#include <afxwin.h>
#include <tchar.h>
#include <cpp/poppler-document.h>
#include <cpp/poppler-page.h>
#include <cpp/poppler-page-renderer.h>
#include <memory>

#include <PDFSpecImports.h>

#pragma comment(lib, "PDFSpec.lib")

CWinApp theApp;

int _tmain(int argc, TCHAR** argv) {
	if (!AfxWinInit(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW)) {
		return 1;
	}
	{
		// "C:/A/\xe3\x81\x82.pdf"
		// "C:\\A\\\xe3\x81\x82.pdf"
		std::unique_ptr<poppler::document> doc(poppler::document::load_from_file("C:\\A\\\xe3\x81\x82.pdf"));
		auto title = doc->get_title();
		auto subject = doc->get_subject();
		auto keywords = doc->get_keywords();
		auto author = doc->get_author();
		auto numPages = doc->pages();
		std::unique_ptr<poppler::page> page(doc->create_page(0));
		auto image = poppler::page_renderer().render_page(page.get());
		std::string fmt = "png";
		image.save("C:/A/A.png", fmt);
	}
	return 0;
}
