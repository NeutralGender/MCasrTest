#pragma once

#include <string>
#include <stg/mdp/mdd/auth/auth.hpp>

#include <stg/mdp/mdd/crypt/symmetric/aes_iv.hpp>
#include <stg/mdp/mdd/crypt/asymmetric/asymmetric.hpp>
#include <stg/mdp/mdd/crypt/hash/hash.hpp>
#include <stg/mdp/mdd/net/unix/client/tcp_client.hpp>

namespace stg::mdp::mdd::auth
{

typedef unsigned char byte;

using stg::mdp::mdd::net::s_unix::client::tcp_client;
using stg::mdp::mdd::crypt::asymmetric::asymmetric;
using stg::mdp::mdd::crypt::symmetric::aes_iv;
using stg::mdp::mdd::crypt::hash::hash;

class auth_multicast
      :public auth_t
{
public:
    auth_multicast(const std::string server_pub_path_,
                   const std::string client_login_):
             auth_t(server_pub_path_, client_login_) 
    {}
    virtual ~auth_multicast() 
    {}

    virtual void auth_client(aes_iv& AES, asymmetric& server, asymmetric& client, hash& sha,
                             tcp_client& cli_socket,  std::vector<byte>& permanent_key,
                             std::vector<byte>& permanent_iv, std::vector<byte>& AES_Ticket)
                             override;

};

void auth_multicast::auth_client(aes_iv& AES, asymmetric& server,asymmetric& client, hash& sha,
                                 tcp_client& cli_socket,
                                 std::vector<byte>& permanent_key,
                                 std::vector<byte>& permanent_iv,
                                 std::vector<byte>& AES_Ticket)
    {
// Load Bob Public Key from file( me must trust this key )
    server.load_pub_key_from_file( server_pub_path );
// End Load Bob Public Key from file( me must trust this key )
    
// Generate RSA key pair for new connection to server
    client.key_generation();
// End Generate RSA key pair for new connection to server

// Send Alice Login Encrypted By Bob Public Key
    std::string alice_login; // alice_login ciphertext is encoded in Hex
    server.encrypt( "Alice", alice_login );

        //std::cout << "alice_login: " << alice_login.size() << std::endl;
    cli_socket.send( alice_login );
// End Send Alice Login Encrypted By Bob Key

// Send Alice public_key To Bob
    std::string Alice_public_key; // Key is encoded in Base64
    client.save_pub_key_to_string( Alice_public_key );

    sleep(1);

        //std::cout << "Alice_public_key: " << Alice_public_key.size() << std::endl;
    cli_socket.send( Alice_public_key );
// END Send Alice public_key To Bob

// Receive Encrypted by Alice public and Signed by Bob private Key from Bob
    sleep(1);

    std::string recv_message;
    recv_message.resize(768);

    cli_socket.receive( recv_message );
    const std::vector<byte> AES_KEY( server.verify( client.decrypt( recv_message ) ) );
// End Receive Encrypted by Alice public and Signed by Bob private Key from Bob

// Receive Encrypted Signed IV by private key from Bob
    sleep(1);

    cli_socket.receive( recv_message );
    const std::vector<byte>AES_IV( server.verify( client.decrypt( recv_message ) ) );
// End Receive Encrypted Signed IV by private key from Bob

// Receive Encrypted Signed Ticket by private key from Bob
    sleep(1);

    cli_socket.receive( recv_message );
    AES_Ticket = ( server.verify( client.decrypt( recv_message ) ) );
// End Receive Encrypted Signed Ticket by private key from Bob

// Calculate Digest: SHA3_256( AES_Ticket + password );
    std::string digest;
    digest.reserve( 256 );

    sha.add_data( std::string{AES_Ticket.begin(), AES_Ticket.end()}+"I am Client" );
    sha.set_digest_size( digest );
    sha.calculate_digest( digest );
        //std::cout << digest << std::endl;
// End Calculate Digest: SHA3_256( AES_Ticket + password );

// Encrypt Hash3_256 by Ephemere Key Received from Bob and Send it to him
    std::string aes_cipher;
    AES.encrypt( AES_KEY, AES_IV, digest, aes_cipher );

    sleep(1);

        //std::cout << "aes_cipher: " << aes_cipher.size() << std::endl;
    cli_socket.send( aes_cipher );
// End Encrypt Hash3_256 by Ephemere Key Received from Bob

// Receive AES Permanent Key form Bob
    sleep(1);

    recv_message.resize(32);
    cli_socket.receive( recv_message );

    AES.decrypt( AES_KEY, AES_IV, recv_message, permanent_key );

    std::cout << "AES_KEY: ";
    std::cout << permanent_key.data() << std::endl;
// END Receive AES Permanent Key form Bob

// Receive AES Permanent IV from Bob
    recv_message.resize(32);
    cli_socket.receive( recv_message );

    AES.decrypt( AES_KEY, AES_IV, recv_message, permanent_iv );

    std::cout << "AES_IV: ";
    std::cout << permanent_iv.data();
    std::cout << std::endl;
// End Receive AES Permanent IV from Bob



    }


} // namespace stg::mdp::mdd::auth

