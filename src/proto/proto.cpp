/*
 * HQN protocol class
 */
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

class proto {
public:
	/**
	 * Based from here
	 * https://kahdev.wordpress.com/2009/02/15/verifying-using-a-certificate-store/
	 */
	static int cert_verify(X509 *cert) {
		int result = 0;

		try {
			X509_STORE *store = X509_STORE_new();

			//TODO chg here to path from settings
			loadToStore("etc/ssl/device.pem", store); // server cert
			loadToStore("etc/ssl/rootCA.pem", store); // ca cert

			// Create the context to verify the certificate.
			X509_STORE_CTX *ctx = X509_STORE_CTX_new();

			// Initial the store to verify the certificate.
			X509_STORE_CTX_init(ctx, store, cert, NULL);

			result = X509_verify_cert(ctx);

			X509_STORE_CTX_cleanup(ctx);
			X509_STORE_CTX_free(ctx);
			X509_STORE_free(store);
			ctx = NULL;
			store = NULL;
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << "\n";
		}
		return result;
	}

	/**
	 * Next one example
	 * http://www.zedwood.com/article/c-libssl-verify-x509-certificate
	 */
	static int sig_verify(const char *cert_pem, const char *intermediate_pem) {
		int result = 0;
		try {
			if (std::strlen(cert_pem) > 0
					&& std::strlen(intermediate_pem) > 0) {
				BIO *b = BIO_new(BIO_s_mem());
				BIO_puts(b, intermediate_pem);
				X509 *issuer = PEM_read_bio_X509(b, NULL, NULL, NULL);
				EVP_PKEY *signing_key = X509_get_pubkey(issuer);

				BIO *c = BIO_new(BIO_s_mem());
				BIO_puts(c, cert_pem);
				X509 *x509 = PEM_read_bio_X509(c, NULL, NULL, NULL);

				result = X509_verify(x509, signing_key);

				EVP_PKEY_free(signing_key);
				BIO_free(b);
				BIO_free(c);
				X509_free(x509);
				X509_free(issuer);
			}
		} catch (const std::exception &ex) {
			std::cerr << "Exception: " << ex.what() << "\n";
		}

		return result;
	}
private:

	/**
	 * Based from here
	 * https://kahdev.wordpress.com/2009/02/15/verifying-using-a-certificate-store/
	 */
	static void loadToStore(std::string file, X509_STORE *&store) {
		X509 *cert = loadCert(file);
		if (cert != NULL) {
			X509_STORE_add_cert(store, cert);
		} else {
			std::cerr << "Can not load certificate " << file << std::endl;
		}
	}

	static X509* loadCert(std::string file) {
		FILE *fp = fopen(file.c_str(), "r");
		X509 *cert = PEM_read_X509(fp, NULL, NULL, NULL);
		fclose(fp);
		return cert;
	}
};

