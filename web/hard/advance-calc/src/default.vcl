vcl 4.0;
import blob;
import cookie;


backend default {
    .host = "127.0.0.1";
    .port = "5000";
}

backend bot {
    .host = "127.0.0.1";
    .port = "3000";
}


sub vcl_recv {

    // if request is for bot return to bot backend
    if (req.url ~ "^/bot") {
        set req.backend_hint = bot;
    } else {
        set req.backend_hint = default;
    }
   
    
    // https://stackoverflow.com/questions/24787052/varnish-cache-with-some-cookies
    if (req.http.Cookie ~ "calc_session=") {
        set req.http.X-Calc-Session = regsub(req.http.Cookie, ".*calc_session=([^;]+);?.*", "\1");
        
    } 
    
    if (req.url ~ "calc_session="){
        set req.http.X-Calc-Session = regsub(req.url, ".*calc_session=([^&]+).*", "\1");
        set req.url = regsub(req.url, "(\?|&)?calc_session=[^&]*", "");
    }
     
    unset req.http.Cookie;


    if (req.url ~ "^/embed/") {
        return(hash);
    } 
    return(pass);
}

sub vcl_backend_fetch {
    if (bereq.http.X-Calc-Session) {
        set bereq.http.Cookie = "calc_session=" + bereq.http.X-Calc-Session;
    }
}

sub vcl_backend_response {
    set beresp.ttl = 10m;  
    set beresp.http.Cache-Control = "public, max-age=600";

    if(beresp.http.Set-Cookie ~ "calc_session=") {
        set beresp.http.X-Calc-Session = blob.transcode(encoded=regsub(beresp.http.Set-Cookie, ".*calc_session=([^;]+);?.*", "\1"), decoding=URL);
        unset beresp.http.Set-Cookie; // for caching
    }
}


sub vcl_deliver {
    if (resp.http.X-Calc-Session) {
        set resp.http.Set-Cookie = "calc_session=" + resp.http.X-Calc-Session+ "; Path=/";
    }
}