/**
 * @file main.c (VUT FIT - ISA projekt)
 * 
 * @brief Atom & RSS 2.0 feed reader
 * @date 2022-10-23
 * @author Karel Jirgl (xjirgl01)
 * @update 2022-11-14
 */

#include <stdio.h>
#include <stdlib.h>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include <libxml/parser.h>
#include <libxml/tree.h>


#define NETWORK_BUFFER_SIZE 1024
#define NEWTORK_TEMPFILE_NAME "temp_file.txt"
#define FILE_LINE_MAX_SIZE 1024



// Parsed Args
char * feedurl = NULL;
char * feedfile = NULL;
char * certfile = NULL;
char * certaddr = NULL;
// If any of these flags below is set, every record is separated by empty line
int T_flag = 0; // Show update or publish time (if listed)
int a_flag = 0; // Show author name or email address (if listed)
int u_flag = 0; // Show URL (if listed)



// Feed Item Info
int feed_or_rss = -1;
xmlChar * feedTitle = NULL;
xmlChar * itemTitle = NULL;
xmlChar * itemLink = NULL;
xmlChar * itemUpdated = NULL;
xmlChar * itemAuthor = NULL;
// Feed Parser error
int parser_error = 0;

int parseElementsRecursively(xmlNode * a_node, int element_level, const char * parent_node_name)
{
	element_level++;

    xmlNode *cur_node = NULL;
    xmlNode *cur_node_sub = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
			// TEST prints
            //printf("(%d) node type: Element, name: %s\n", element_level, cur_node->name);
            //printf("node type: Element, name: %s, value: %s\n", cur_node->name, xmlNodeGetContent(cur_node));

			if (element_level == 1) {
				if (strcmp((const char *)cur_node->name, "feed") == 0) {
					feed_or_rss = 0;
				} else if (strcmp((const char *)cur_node->name, "rss") == 0) {
					feed_or_rss = 1;
				} else {
					parser_error = 1;
				}
			} else {
				if (feed_or_rss == 0) {
				// FEED ATOM

					if (element_level == 2) { // get feed title

						if (strcmp((const char *)cur_node->name, "title") == 0) {
							if (feedTitle == NULL) {
								feedTitle = xmlNodeGetContent(cur_node);
							}
						}

					} else if (element_level == 3 && strcmp(parent_node_name, "entry") == 0) {

						if (strcmp((const char *)cur_node->name, "title") == 0) { // get item title

							// Print feed title and empty line
							if (itemTitle == NULL) { // if it is the first item title -> print feedTitle first
								printf("*** %s ***\n", feedTitle);
							} else if (T_flag == 1 || a_flag == 1 || u_flag == 1) { // if it is not the first item title -> check empty line print
								printf("\n");
							}

							// Print item and free memory
							if (itemTitle != NULL) {
								printf("%s\n", itemTitle);
								xmlFree(itemTitle);
								itemTitle = NULL;
							}
							if (u_flag == 1) {
								if (itemLink != NULL) {
									printf("URL: %s\n", itemLink);
									xmlFree(itemLink);
									itemLink = NULL;
								}
							}
							if (T_flag == 1) {
								if (itemUpdated != NULL) {
									printf("Aktualizace: %s\n", itemUpdated);
									xmlFree(itemUpdated);
									itemUpdated = NULL;
								}
							}
							if (a_flag == 1) {
								if (itemAuthor != NULL) {
									printf("Autor: %s\n", itemAuthor);
									xmlFree(itemAuthor);
									itemAuthor = NULL;
								}
							}

							// save new item Title
							itemTitle = xmlNodeGetContent(cur_node);

						} else if (strcmp((const char *)cur_node->name, "link") == 0) { // get link - only link element exists
							if (itemLink == NULL) {
								itemLink = xmlGetProp(cur_node, (const xmlChar *)"href");
							}
						} else if (strcmp((const char *)cur_node->name, "published") == 0) { // get updated (or published if updated not found)
							if (itemUpdated == NULL) {
								itemUpdated = xmlNodeGetContent(cur_node);
							}
						} else if (strcmp((const char *)cur_node->name, "updated") == 0) { // get updated (or published if updated not found)
							if (itemUpdated == NULL) {
								itemUpdated = xmlNodeGetContent(cur_node);
							} else { // if updated is already set -> free and set new value
								xmlFree(itemUpdated);
								itemUpdated = NULL;
								itemUpdated = xmlNodeGetContent(cur_node);
							}
						} else if (strcmp((const char *)cur_node->name, "author") == 0) { // get author's name or email
							for (cur_node_sub = cur_node->children; cur_node_sub; cur_node_sub = cur_node_sub->next) {
								if (strcmp((const char *)cur_node_sub->name, "name") == 0) { // get author's name 
									if (itemAuthor == NULL) {
										itemAuthor = xmlNodeGetContent(cur_node_sub);
									}
								}
							}
							// if author's name not found -> try to find email
							if (itemAuthor == NULL) {
								itemAuthor = xmlNodeGetContent(cur_node);
							}
						}

					}



				} else {
				// RSS 2.0

					if (element_level == 3) { // get feed title

						if (strcmp((const char *)cur_node->name, "title") == 0) {
							if (feedTitle == NULL) {
								feedTitle = xmlNodeGetContent(cur_node);
							}
						}

					} else if (element_level == 4 && strcmp(parent_node_name, "item") == 0) {

						if (strcmp((const char *)cur_node->name, "title") == 0) { // get item title
							
							// Print feed title and empty line
							if (itemTitle == NULL) { // if it is the first item title -> print feedTitle first
								printf("*** %s ***\n", feedTitle);
							} else if (T_flag == 1 || a_flag == 1 || u_flag == 1) { // if it is not the first item title -> check empty line print
								printf("\n");
							}

							// Print item and free memory
							if (itemTitle != NULL) {
								printf("%s\n", itemTitle);
								xmlFree(itemTitle);
								itemTitle = NULL;
							}
							if (u_flag == 1) {
								if (itemLink != NULL) {
									printf("URL: %s\n", itemLink);
									xmlFree(itemLink);
									itemLink = NULL;
								}
							}
							if (T_flag == 1) {
								if (itemUpdated != NULL) {
									printf("Aktualizace: %s\n", itemUpdated);
									xmlFree(itemUpdated);
									itemUpdated = NULL;
								}
							}
							if (a_flag == 1) {
								if (itemAuthor != NULL) {
									printf("Autor: %s\n", itemAuthor);
									xmlFree(itemAuthor);
									itemAuthor = NULL;
								}
							}

							// save new item Title
							itemTitle = xmlNodeGetContent(cur_node);

						} else if (strcmp((const char *)cur_node->name, "link") == 0) { // get link - only link element exists
							if (itemLink == NULL) {
								itemLink = xmlNodeGetContent(cur_node);
							}
						} else if (strcmp((const char *)cur_node->name, "pubDate") == 0) { // get update - only pubDate element exists
							if (itemUpdated == NULL) {
								itemUpdated = xmlNodeGetContent(cur_node);
							}
						} else if (strcmp((const char *)cur_node->name, "author") == 0) { // only author element exists
							if (itemAuthor == NULL) {
								itemAuthor = xmlNodeGetContent(cur_node);
							}
						}

					}

				}

			}

        }

		// If no error -> go recursively to children elements
		if (parser_error == 0) {
        	parseElementsRecursively(cur_node->children, element_level, (const char *)cur_node->name);
		}
	}



	// on TOP element finished
	if (element_level == 1) {
		
		// Check file structure
		if (feedTitle == NULL) {
			parser_error = 2;
		}
		if (itemTitle == NULL) {
			parser_error = 3;
		}
		if (feedTitle == NULL && itemTitle == NULL) {
			parser_error = 4;
		}


		if (T_flag == 1 || a_flag == 1 || u_flag == 1) { // if it is not the first item title -> check empty line print
			printf("\n");
		}

		// Print last item and free memory
		if (itemTitle != NULL) {
			printf("%s\n", itemTitle);
			xmlFree(itemTitle);
			itemTitle = NULL;
		}
		if (u_flag == 1) {
			if (itemLink != NULL) {
				printf("URL: %s\n", itemLink);
				xmlFree(itemLink);
				itemLink = NULL;
			}
		}
		if (T_flag == 1) {
			if (itemUpdated != NULL) {
				printf("Aktualizace: %s\n", itemUpdated);
				xmlFree(itemUpdated);
				itemUpdated = NULL;
			}
		}
		if (a_flag == 1) {
			if (itemAuthor != NULL) {
				printf("Autor: %s\n", itemAuthor);
				xmlFree(itemAuthor);
				itemAuthor = NULL;
			}
		}


		// Free feed title
		xmlFree(feedTitle);
		feedTitle = NULL;

		return parser_error;
	}

	return 0;
}


/* Name: feedFileParse() (original: main() - Simple example to parse a file)
*  Source: http://www.xmlsoft.org/examples/tree1.c
*  Date: 2022-11-13
*  Author: Dodji Seketeli
*  Edited: true
*  License: see Copyright for the status of this software (https://gitlab.gnome.org/GNOME/libxml2/-/blob/master/Copyright)
*/
int feedFileParse(const char *filename) {
	xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
	int parse_error = 0;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(filename, NULL, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);

    if (doc == NULL) {
        fprintf(stderr, "ERROR(parser): could not parse file %s\n", filename);
		parse_error = -1;
    } else {

		/*Get the root element node */
		root_element = xmlDocGetRootElement(doc);

		parse_error = parseElementsRecursively(root_element, 0, "TOP");

		// CHECK ERRORS
		if (parser_error == 1) {
			fprintf(stderr, "ERROR(parser): wrong top element name (<feed> or <rss>)\n");
		} else if (parser_error > 1) {
			fprintf(stderr, "ERROR(parser): no feed title found or any article found\n");
		}
	}

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

	return parse_error;
}
/* ******* feedFileParse() - end ******* */


/* Name: donwloadFeedFile() (original: tls_connect())
*  Source: https://github.com/theno/openssl-examples/blob/master/openssl-examples/tls-connect.c
*  Date: 2022-11-05
*  Author: theno
*  Edited: true
*  License: https://github.com/theno/openssl-examples/blob/master/LICENSE.md (https://github.com/openssl/openssl/blob/master/LICENSE.txt)
*/
int donwloadFeedFile(char *https, char *hostname, char *path, char *fileName)
{
	// TEST prints
	//printf("https: %s\n", https);
	//printf("hostname: %s\n", hostname);
	//printf("path: %s\n", path);


    /* SSL context structure */
    SSL_CTX *ctx;
    /* SSL connection structure */
    SSL *ssl;
    /* connection BIO */
    BIO *cbio;

    /* addressee of the connection */
    char name[NETWORK_BUFFER_SIZE];
    /* request */
    char req[NETWORK_BUFFER_SIZE];
    /* response */
    char resp[NETWORK_BUFFER_SIZE];
    int len;


	int response_code = 200;

	
	/* name = "<hostname>:<port>" */
	sprintf(name, "%s:%s", hostname, https);

	// SSL
	if(https[4] == 's') {
		/* Set up the OpenSSL library */
		SSL_load_error_strings();
		SSL_library_init();

		//OpenSSL_add_all_algorithms();

		/* Create SSL context structure and load the trust store
		 * (accepted root ca-certificates) */
		ctx = SSL_CTX_new(SSLv23_client_method());
		if(! ctx) {
			fprintf(stderr, "Error creating SSL context\n");
			ERR_print_errors_fp(stderr);
			exit(1);
		}

		if (certfile != NULL || certaddr != NULL) {
			if(! SSL_CTX_load_verify_locations(ctx, certfile, certaddr)) {
				fprintf(stderr, "Error loading certfile/certaddr into SSL context\n");
				ERR_print_errors_fp(stderr);
				exit(1);
			}
		} else {
			if(! SSL_CTX_set_default_verify_paths(ctx)) {
				fprintf(stderr, "Error loading trust store into SSL context\n");
				ERR_print_errors_fp(stderr);
				exit(1);
			}
		}
		
		
		
		/* Set up the SSL connection */
		cbio = BIO_new_ssl_connect(ctx);
		

		/* Set flag SSL_MODE_AUTO_RETRY */
		BIO_get_ssl(cbio, &ssl);
		SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
		

		/* Connect to server <hostname> */
		BIO_set_conn_hostname(cbio, name);
		

		if(BIO_do_connect(cbio) <= 0) {
			fprintf(stderr, "Error attempting to connect\n");
			ERR_print_errors_fp(stderr);
			BIO_free_all(cbio);
			SSL_CTX_free(ctx);
			exit(1);
		}


		/* Check the certificate */
		if(SSL_get_verify_result(ssl) != X509_V_OK) {
			fprintf(stderr,
					"Certificate verification error: %i\n",
					(int) SSL_get_verify_result(ssl));
			/* continue */
		}
	
	// NO SSL
	} else {
		/* Connect to server <hostname> */
		cbio = BIO_new_connect(name);
		
		
		if (BIO_do_connect(cbio) <= 0) {
			fprintf(stderr, "Error connecting to server\n");
			ERR_print_errors_fp(stderr);
			exit(1);
		}
	}





    /* send HTTP request to the server <hostname> */
    sprintf(req, "GET /%s HTTP/1.1\x0D\x0AHost: %s\x0D\x0A\x43onnection: Close\x0D\x0A\x0D\x0A", path, hostname);
    BIO_puts(cbio, req);



    /* read HTTP response from server and print to stdout */
	FILE *pfile;
	pfile = fopen(fileName, "w+");
	if (pfile != NULL) {

		// READ RESPONSE LOOP
		int headers_out = 0;
		len = 0;
		do {
			resp[len] = '\0';

			// Check if http headers have been read -> if not -> remove them from response
			if (headers_out == 0) {

				// find end of http headers
				char *headers_end_pos = strstr(resp, "\r\n\r\n");
				
				// if found -> write response data without http headers to the file
				if (headers_end_pos != NULL) {
					headers_out = 1;
					fprintf(pfile, "%s", headers_end_pos+4);
				}

				// GET HTTP STATUS CODE
				char *http_code = strstr(resp, "HTTP/1.1 ");
				if(http_code != NULL) {
					http_code = strchr(http_code, ' ');
					if (http_code != NULL) {
						http_code++;
						char temp[4] = "";
						temp[0] = *(http_code);
						temp[1] = *(http_code+1);
						temp[2] = *(http_code+2);
						temp[3] = '\0';
						response_code = atoi(temp);
					}
				}

			// -> else -> write data to the file
			} else {
				fprintf(pfile, "%s", resp);
			}

			// Read response data
			len = BIO_read(cbio, resp, NETWORK_BUFFER_SIZE);
		} while(!(len <= 0));

	} else {
		fprintf(stderr, "ERROR(download): Unable to create a temp file!\n");
		response_code = -1;
	}
	fclose(pfile);



	
    /* close TCP/IP connection and free bio and context */
    BIO_free_all(cbio);
	if(https[4] == 's') {
		SSL_CTX_free(ctx);
	}
    return response_code;
}
/* ******* donwloadFeedFile() - end ******* */


char * url_separate_path(char *url)
{
	char *temp_url = strchr(url, '/');
	if(temp_url != NULL) {
		*temp_url = '\0';
		temp_url++;
		return temp_url;
	}
	return url+strlen(url);
}


int parseArgs(int argc, char *argv[])
{
	if (argc == 2) {
		feedurl = argv[1];
	} else if (argc > 2) {
		int argStart = 1;
		if (argv[1][0] == '-' && argv[1][1] == 'f') {
			feedfile = argv[2];
			argStart = 3;
		} else {
			feedurl = argv[1];
			argStart = 2;
		}
		while(argStart < argc) {
			if (argv[argStart][0] == '-' && argv[argStart][1] == 'T') {
				T_flag = 1;
			} else if (argv[argStart][0] == '-' && argv[argStart][1] == 'a') {
				a_flag = 1;
			} else  if (argv[argStart][0] == '-' && argv[argStart][1] == 'u') {
				u_flag = 1;
			} else  if (argv[argStart][0] == '-' && argv[argStart][1] == 'c') {
				if (argStart+1 < argc) {
					certfile = argv[argStart+1];
				} else {
					fprintf(stderr, "ERROR(usage): missing cert param\n");
					return 1;
				}
			} else  if (argv[argStart][0] == '-' && argv[argStart][1] == 'C') {
				if (argStart+1 < argc) {
					certaddr = argv[argStart+1];
				} else {
					fprintf(stderr, "ERROR(usage): missing cert param\n");
					return 1;
				}
			}

			argStart++;
		}
	}

	return 0;
}


int prepareAndDownloadFile(char * feedFileURL)
{
	int response_code = 200;
	if(strncmp(feedFileURL, "https://", 8) == 0) {
		response_code = donwloadFeedFile("https", feedFileURL+8, url_separate_path(feedFileURL+8), NEWTORK_TEMPFILE_NAME);
	} else if(strncmp(feedFileURL, "http://", 7) == 0) {
		response_code = donwloadFeedFile("http", feedFileURL+7, url_separate_path(feedFileURL+7), NEWTORK_TEMPFILE_NAME);
	} else {
		fprintf(stderr, "ERROR(usage): ./feedreader http(s)://<hostname>/<path> <args>\n");
		return 1;
	}
	return response_code;
}


int multipleFeedsDownload(char *feedsFileName)
{
	char line[FILE_LINE_MAX_SIZE];
	FILE *pfile;
	int error_occurred = 0;

	pfile = fopen(feedsFileName, "r");
	if (pfile == NULL) {
		fprintf(stderr, "ERROR(multidownload): Could not open file %s!\n", feedsFileName);
		return 1;
	}



	while (fgets(line, FILE_LINE_MAX_SIZE, pfile) != NULL) {
        
		// Trim from front
		char * front = line-1;
    	while(*++front == ' '){};

		// Trim from end
		char * end = front+strlen(front)-1;
		while(*end == '\n' || *end == '\r' || *end == ' ') {
			*end = '\0';
			end--;
		}

		// If not comment
		if(strncmp(front, "#", 1) != 0 && front[0] != '\0') {
			
			// Download file
			printf("----------\nDOWNLOADING: %s\n", front);
			int response_code = prepareAndDownloadFile(front);

			// Check download error
			if (response_code == 1) {
				error_occurred++;
			// Check HTTP error (file moved?)
			} else if (response_code != 200) {
				fprintf(stderr, "ERROR(multidownload): HTTP error: %d\n", response_code);
				error_occurred++;
			} else {

				// Parse feed
				if(feedFileParse(NEWTORK_TEMPFILE_NAME) != 0) {
					error_occurred++;
				}

			}
		}
    }



	fclose(pfile);
	
	return 0;
}


int main(int argc, char *argv[])
{
	// Parse args
	if (parseArgs(argc, argv) != 0) {
		printf("Usage: %s http(s)://<hostname>/<path> <args>\n", argv[0]);
		return (EXIT_FAILURE);
	}

	// Download feed(s)
	int response_code = 200;
	// Download from file
	if (feedfile != NULL) {
		if (multipleFeedsDownload(feedfile) != 0) {
			return (EXIT_FAILURE);
		}
	// Download from URL
	} else {
		response_code = prepareAndDownloadFile(feedurl);

		// Check download error
		if (response_code < 100) {
			return (EXIT_FAILURE);
		
		// Check HTTP error (file moved?)
		} else if (response_code != 200) {
			fprintf(stderr, "ERROR(HTTP): %d\n", response_code);
			return (EXIT_FAILURE);
		} else {
			// Parse feed
			if (feedFileParse(NEWTORK_TEMPFILE_NAME) != 0) {
				return (EXIT_FAILURE);
			}
		}
	}

    return 0;
}
