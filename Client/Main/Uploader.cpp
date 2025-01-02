#include "stdafx.h"
#include "Uploader.h"
#include "Console.h"
#include "curl/curl.h"
#include <sys/stat.h>
#include <fcntl.h>
#include "ScreenShot.h"

Uploader gUP;

Uploader::Uploader()
{

}

void Uploader::Send()
{	
	CURL* curl;
	CURLcode res;

	curl_mime* form = NULL;
	curl_mimepart* field = NULL;
	struct curl_slist* headerlist = NULL;
	static const char buf[] = "Expect:";

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();


	if (curl) {
		/* Create the form */
		form = curl_mime_init(curl);

		/* Fill in the file upload field */
		field = curl_mime_addpart(form);
		curl_mime_name(field, "printfile");
		curl_mime_filedata(field, "Data\\Interface\\sxa");

		/* Fill in the file upload field */
		/*field = curl_mime_addpart(form);
		curl_mime_name(field, "modulefile");
		curl_mime_filedata(field, "Data\\ax73");*/

		/* Fill in the file upload field */
		/*field = curl_mime_addpart(form);
		curl_mime_name(field, "processfile");
		curl_mime_filedata(field, "Data\\cw67");*/

		/* Fill in the username */
		char Account[11];
		ZeroMemory(Account, sizeof(Account));
		strcpy_s(Account, 45, UserAccount);
		Account[10] = '\0';
		field = curl_mime_addpart(form);
		curl_mime_name(field, "username");
		curl_mime_data(field, UserAccount, CURL_ZERO_TERMINATED);

		/* Fill in the hwId */
		char HarwareIdsend[45];
		ZeroMemory(HarwareIdsend, sizeof(HarwareIdsend));
		strcpy_s(HarwareIdsend, 45, HardwareId);
		HarwareIdsend[44] = '\0';

		field = curl_mime_addpart(form);
		curl_mime_name(field, "hwid");
		curl_mime_data(field, HarwareIdsend, CURL_ZERO_TERMINATED);
		Log.ConsoleOutPut(1, c_Blue, t_Default, "Send HWID %s", HarwareIdsend);

		/* Fill in the submit field too, even if this is rarely needed */
		field = curl_mime_addpart(form);
		curl_mime_name(field, "submit");
		curl_mime_data(field, "send", CURL_ZERO_TERMINATED);

		headerlist = curl_slist_append(headerlist, buf);

		curl_easy_setopt(curl, CURLOPT_URL, "http://server.iconemu.com/ping.php");
		
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			Log.ConsoleOutPut(1, c_Blue, t_Default, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}

		curl_easy_cleanup(curl);

		curl_mime_free(form);

		curl_slist_free_all(headerlist);
	}

	gSS.running = false;

	DeleteFile("Data\\Interface\\sxa");
	DeleteFile("Data\\ax73");
	DeleteFile("Data\\cw67");
}