# HARD OSINT CHALLENGE

## CYBER THREAT INVESTIGATION

A phishing campaign used for targeting users of a Web platform  has been identified. The campaign was conduced using an email with a body similar to the following:

```
Received: from CYXPR19MB8312.namprd19.prod.outlook.com (::1) by
 MN0PR19MB6312.namprd19.prod.outlook.com with HTTPS; Thu, 3 Aug 2023 02:15:09
 +0000
Received: from DB8PR06CA0029.eurprd06.prod.outlook.com (2603:10a6:10:100::42)
 by CYXPR19MB8312.namprd19.prod.outlook.com (2603:10b6:930:e5::9) with
 Microsoft SMTP Server (version=TLS1_2,
 cipher=TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384) id 15.20.6631.47; Thu, 3 Aug
 2023 02:15:08 +0000
Received: from DB8EUR05FT062.eop-eur05.prod.protection.outlook.com
 (2603:10a6:10:100:cafe::26) by DB8PR06CA0029.outlook.office365.com
 (2603:10a6:10:100::42) with Microsoft SMTP Server (version=TLS1_2,
 cipher=TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384) id 15.20.6652.19 via Frontend
 Transport; Thu, 3 Aug 2023 02:15:07 +0000
Authentication-Results: spf=softfail (sender IP is 80.96.157.90)
 smtp.mailfrom=winner-win.art; dkim=none (message not signed)
 header.d=none;dmarc=fail action=none header.from=newsletter.otto.de;
Received-SPF: SoftFail (protection.outlook.com: domain of transitioning
 winner-win.art discourages use of 80.96.157.90 as permitted sender)
Received: from qktfxthukmwfnksijbkrjxkzhstbswa.whstr8 (80.96.157.90) by
 DB8EUR05FT062.mail.protection.outlook.com (10.233.239.17) with Microsoft SMTP
 Server id 15.20.6631.45 via Frontend Transport; Thu, 3 Aug 2023 02:15:07
 +0000
X-IncomingTopHeaderMarker:
 OriginalChecksum:E12A17C82168974B504B0CF3CC12F0916439B878AE713C202229DF7707818C9C;UpperCasedChecksum:3A20475CBE5B8F607978BBCDF72535118ADDDA37CE57AFA9D1402DAE96E3271C;SizeAsReceived:483;Count:11
Message-Id: <xxxxxxxx>
From: Satayo Warning Message ⚠️ <xxxxxx@xxxxxx.xxx>
Subject: Warning: Suspicious activities detected
Reply-To: reply_to@xxxxxxx
To: xxxxxx@xxxxxx.xxx
Cc: xxxxxx@xxxxxx.xxx
Content-Transfer-Encoding: 7bit
Content-Type: text/html; charset=UTF-8
Date: Thu, 03 Aug 2023 04:14:33 +0200
Sender: notify<xxxxxx@xxxxxx.xxx>
X-IncomingHeaderCount: 11
Return-Path: xxxxxx@xxxxxx.xxx
X-MS-Exchange-Organization-ExpirationStartTime: 03 Aug 2023 02:15:07.7948
 (UTC)
X-MS-Exchange-Organization-ExpirationStartTimeReason: OriginalSubmit
X-MS-Exchange-Organization-ExpirationInterval: 1:00:00:00.0000000
X-MS-Exchange-Organization-ExpirationIntervalReason: OriginalSubmit
X-MS-Exchange-Organization-Network-Message-Id:
 f2a59e76-4dfc-446a-d23d-08db93c7753f
X-EOPAttributedMessage: 0
X-EOPTenantAttributedMessage: 84df9e7f-e9f6-40af-b435-aaaaaaaaaaaa:0
X-MS-Exchange-Organization-MessageDirectionality: Incoming
X-MS-PublicTrafficType: Email
X-MS-TrafficTypeDiagnostic:
 DB8EUR05FT062:EE_|CYXPR19MB8312:EE_|MN0PR19MB6312:EE_
X-MS-Exchange-Organization-AuthSource:
 DB8EUR05FT062.eop-eur05.prod.protection.outlook.com
X-MS-Exchange-Organization-AuthAs: Anonymous
X-MS-UserLastLogonTime: 8/3/2023 1:50:08 AM
X-MS-Office365-Filtering-Correlation-Id: f2a59e76-4dfc-446a-d23d-08db93c7753f
X-MS-Exchange-EOPDirect: true
X-Sender-IP: 80.96.157.90
X-SID-PRA: xxxxxx@xxxxxx.xxx
X-SID-Result: NONE
X-MS-Exchange-Organization-PCL: 2
X-MS-Exchange-Organization-SCL: 7
X-Microsoft-Antispam: BCL:8;
X-MS-Exchange-CrossTenant-OriginalArrivalTime: 03 Aug 2023 02:15:07.6699
 (UTC)
X-MS-Exchange-CrossTenant-Network-Message-Id: f2a59e76-4dfc-446a-d23d-08db93c7753f
X-MS-Exchange-CrossTenant-Id: 84df9e7f-e9f6-40af-b435-aaaaaaaaaaaa
X-MS-Exchange-CrossTenant-AuthSource:
 DB8EUR05FT062.eop-eur05.prod.protection.outlook.com
X-MS-Exchange-CrossTenant-AuthAs: Anonymous
X-MS-Exchange-CrossTenant-FromEntityHeader: Internet
X-MS-Exchange-CrossTenant-RMS-PersistedConsumerOrg:
 00000000-0000-0000-0000-000000000000
X-MS-Exchange-Transport-CrossTenantHeadersStamped: CYXPR19MB8312
X-MS-Exchange-Transport-EndToEndLatency: 00:00:02.3153826
X-MS-Exchange-Processed-By-BccFoldering: 15.20.6631.046
X-Microsoft-Antispam-Mailbox-Delivery:
	abwl:0;wl:1;pcwl:1;kl:0;dwl:0;dkl:0;rwl:0;ucf:0;jmr:0;ex:0;psp:0;auth:0;dest:I;OFR:TrustedSenderList;ENG:(5062000305)(920221119095)(90000117)(920221120095)(90012020)(91020020)(91040095)(9050020)(9075021)(9100338)(944500132)(2008001134)(4810010)(4910033)(8820095)(9610025)(9525003)(10145022)(9439006)(9310011)(9220031)(120001);
X-Message-Delivery: Vj0xLjE7dXM9MDtsP==
MIME-Version: 1.0

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:v="urn:schemas-microsoft-com:vml" xmlns:o="urn:schemas-microsoft-com:office:office">
<head>
    <base target="_blank" />
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=0">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-status-bar-style" content="black">
    <meta name="format-detection" content="telephone=no">
    <meta name="robots" content="noindex, nofollow">
    <meta name="x-apple-disable-message-reformatting">
    <!--[if gte mso 9]><xml><o:OfficeDocumentSettings><o:AllowPNG/><o:PixelsPerInch>96</o:PixelsPerInch></o:OfficeDocumentSettings></xml><![endif]-->
    <title>Satayo Warning </title>
    <!--  -->
    <style>
        body, #body-table, .body-table { height: 100% !important; width: 100% !important; margin: 0; padding: 0; font-family:SST, SST-Roman, Arial, Helvetica, sans-serif; min-width: 100%!important; }
        a { text-decoration: none; }
        img, a img { border: 0; outline: none; text-decoration: none; }
        table, td, th { border-collapse: collapse; mso-table-lspace: 0pt; mso-table-rspace: 0pt; }
        .ReadMsgBody { width: 100%; }
        .ExternalClass { width: 100%; }
        .ExternalClass, .ExternalClass p, .ExternalClass span, .ExternalClass font, .ExternalClass td, .ExternalClass div { line-height: 100%; } 
        img { -ms-interpolation-mode: bicubic; } 
        body, table, td, p, a, li, blockquote { -ms-text-size-adjust: 100%; -webkit-text-size-adjust: 100%; } 
        #outlook a { padding: 0; }
        @media only screen and (max-width: 480px) { body { margin: 0 !important; } div[style*="margin: 16px 0"] { margin: 0 !important; font-size: 100% !important; }}
        
        /*CTA HOVER STYLES*/
        .cta-hover_CD3D0F_ON {transition: 0.1s;}
        .cta-hover_2D64E6_ON {transition: 0.1s;}
        .cta-hover_CD3D0F_ON:hover {box-shadow: 0px 0px 0px 3px white inset, 0px 0px 0px 1px #CD3D0F!important;}
        .cta-hover_2D64E6_ON:hover {box-shadow: 0px 0px 0px 3px white inset, 0px 0px 0px 1px #2D64E6!important;}
        
        @media only screen and (max-width:480px) {
            .body, #body-table {background-color:#eeeeee!important;}
            
            .hide { display: none!important; }
            .no-bg { background: none!important; }
            .mobile_image { display: block !important;  max-height: none !important;}
            
            .responsive { width: 100% !important; }
            .responsiveInner { width: 85% !important; }
            .ps_logo {width:50px!important; height: 39px!important;}
            .w100 {width: 100%!important;}
            .w95 {width: 95%!important;}
            .w90 {width: 90%!important;}
            .w85 {width: 85%!important;}
            .w80 {width: 80%!important;}
            .w75 {width: 75%!important;}
            .w70 {width: 70%!important;}
            .w65 {width: 65%!important;}
            .w60 {width: 60%!important;}
            .w55 {width: 55%!important;}
            .w50 {width: 50%!important;}
            .w46 {width: 46%!important;}
            .w47 {width: 47%!important;}
            .w48 {width: 48%!important;}
            .w45 {width: 45%!important;}
            
            .block { display: block !important; }
            .display-table {display: table!important;}
            .inline-block { display: inline-block !important; }
            .display-header {display: table-header-group!important}
            .display-footer {display: table-footer-group!important}
            .float-l {float: left!important;}
            .float-r {float: right!important;}
            .cntr {float: none !important; margin: auto !important; text-align: center !important;}
            #txt-align-cntr {text-align: center!important; margin: auto 0!important; float: none!important;}
            
            .hauto { height: auto !important; }
            .h135 { height: 135px !important; }
            .h30 { height: 30px !important; }
            .h25 { height: 25px !important; }
            .h20 { height: 20px !important; }
            .h15 { height: 15px !important; }
            .h100per {height: 100%!important;}
            
            .p-0 {padding: 0!important;}
            .p-b-0 { padding-bottom: 0!important;}
            .p-t-0 { padding-top: 0!important;}
            .p-l-0 { padding-left: 0!important;}
            .p-r-0 { padding-right: 0!important;}
            .p-b-header {padding-bottom: 7%!important;}
            .p-t-header {padding-top: 0%!important;} .p-20 { padding: 20px !important; }
            .p-t-30 { padding-top: 30px !important; }
            .p-b-30 { padding-bottom: 30px !important; }
            .p-trl-30 { padding-top: 30px !important; padding-right: 30px !important; padding-left: 30px !important; }
            .p-t-100per {padding-top: 100%!important;}
            .p-t-10 {padding-top: 10px!important;}
            .p-t-20 {padding-top: 20px!important;}
            .p-t-30 {padding-top: 30px!important;}
            .p-t-40 {padding-top: 40px!important;}
            
            .m-0-auto { margin: 0 auto !important; }
            .m-t-10 { margin-top: 10px !important; }
            .m-t-20 { margin-top: 20px !important; }
            .m-t-30 { margin-top: 30px !important; }
            .m-b-10 { margin-bottom: 10px !important; }
            .m-b-20 { margin-bottom: 20px !important; }
            .m-b-30 { margin-bottom: 30px !important; }
            .m-trl-30 { margin-top: 30px !important; margin-right: 30px !important; margin-left: 30px !important; }
            
            .fs20 { font-size: 20px !important; }
            .fs18 { font-size: 18px !important; }
            .fs16 { font-size: 16px !important; }
            .fs14 { font-size: 14px !important; }
            
            .mobbg-reset { background-position: top center!important;}
            .mobbg-reset-header { background-position: 50% 50%!important;}
            .mobbg-reset-top { background-position: top center!important;}
            .mobbg-reset-bottom { background-position: bottom center!important;}
            .footer-links {padding: 5px 0px!important;}
            
            /*MOBILE IVALDI CSS*/
            .imageindent_spacerTop_hide_optional_560 {display:none!important;}
            
            /*MOBILE ADJUST FOR MB*/
            .hero-title-mobile-css {font-size:20px!important; line-height:26px!important; }
            .accent-title-mobile-css {font-size:10px!important; line-height:20px!important; }
            .paragraph-mobile-css {font-size:14px!important; line-height:24px!important; }
            .cta-wrapper-mobile-css {min-width:80px;max-width:250px!important; min-height:40px!important; }
            .cta-mobile-css {font-size:14px!important; line-height:17.5px!important;} .cta-mobile-padding-css {padding:11px!important;}
            .cta-width-spacer {width:80px!important;}
            .cta-height-spacer {height:40px!important;}
            .disclaimer-mobile-css{}
            .mobile-module-spacers { height:30px!important;}
            .mobile-module-inner-spacing-bottom-10 { padding-bottom:10px!important;}
            .mobile-module-inner-spacing-bottom-20 { padding-bottom:20px!important;}
            .mobile-module-inner-spacing-bottom-30 { padding-bottom:30px!important;}
            .mobile-module-inner-spacing-subtitle {padding-bottom:10px!important;}
        }
        
        @media only screen and (max-width:480px) {
            
            u + .body .gmail-hide {display: none!important;}
            body[data-outlook-cycle] { margin: 0!important; width: 100%!important; padding: 0!important;}
            body[data-outlook-cycle] .gmail-hide {display:none!important;}
            
            div#menu {left:0;width:100%;max-height:0!important;overflow:hidden!important;top:40px; transition: all  0.5s!important;} 
            table.menu th { display: block!important; border-top:0!important;background: none!important; height: auto!important; padding: 5px!important;}
            
            div#menu th:first-child {border-top:0!important;background: none!important;}  
            div#menu th:last-child {border-top:0!important;border-bottom:0px!important; background: none!important;}
            
            label.hamburger {cursor:pointer;display: inline-block!important;position:inherit!important;right:0;top:0px!important;z-index:1;padding:0px 0px!important; float: right!important; width: 100%!important;}
            label.hamburger img {width:34px!important;height:42px!important; float: right!important; margin-right: 10px!important; text-align: right!important; display: block!important;}
            #hidden-checkbox:checked ~ div#menu {max-height:300px!important; display: block!important; padding-bottom: 20px!important}
            .nav-bgcolor {background: rgba(0,0,0,0.4)!important;}
            
        }
        
        
        
        @media only screen and (max-width:320px) {
            .responsive-bp320 { width: 100% !important; }
            .w100-bp320 { width: 100% !important; }
            .w85-bp320 { width: 85% !important; }
            .w60-bp320 { width: 60% !important; }
            .block-bp320 { display: block !important; }
            .hauto-bp320 { height: auto !important; }
            .text-center-bp320 { text-align: center !important; }
            .cntr-bp320 {float: none !important; margin: auto !important; text-align: center !important;}
            .coverbg-bp320 {background-size: cover!important;}
            .m-b-20-bp320 { margin-bottom: 20px!important;}
            
            .hide-bp320 {display:none!important;}
            .body-table { background-size: 100% 400px!important}
            .no-bg-bp320 {background: none!important;}
            .footer-links {width: 100%!important; padding: 5px 0px!important; display: block!important;}
            .body-bg {background-size: 110%!important;}
            .nav-bgcolor {background: rgba(22,60,110,1)!important;}
            
        }
        
    </style>	
    <!--[if mso]><style> body, #body-table, .body-table, table, td, th, span, font, a {font-family: Arial, Helvetica, sans-serif !important;}</style><![endif]-->
    
    <style></style><style>
        .subjectb_NOT_USED {display:none!important;}
        .subjectc_NOT_USED {display:none!important;}
        .subjectd_NOT_USED {display:none!important;}
    </style>
    
    <!--  -->
    <style>
        @media only screen and (max-width:480px){
            .p-t-20_40px {padding-top: 20px!important; }
        }
    </style>
    
</head>
<body class="body" bgcolor="#FFFFFF" style="padding:0; width: 100%; background:#FFFFFF">
    <center>
        <table class="responsive" border="0" cellpadding="0" cellspacing="0" width="100%" background="#FFFFFF" style="width: 100%; background:#FFFFFF; min-width:100%">
            <tr>
                <td align="center" valign="top" background="#FFFFFF">
                    <table id="main-wrapper" class="responsive" border="0" cellpadding="0" cellspacing="0" align="center" width="740" bgcolor="#EEEEEE">
                        <tr>
                            <td align="center" valign="top" bgcolor="#EEEEEE">
                                <span style="display:none;"> </span>
                                <table id="header-section" class="responsive" border="0" cellpadding="0" cellspacing="0" align="center" width="740">
                                    <tr>
                                        <td class="mobbg-reset-header"  border="0" cellpadding="0" cellspacing="0" align="center" width="740" bgcolor="#af171c" style="background-position: -9999px; background-size: cover; background-repeat: no-repeat;">
                                            <table class="responsive" width="740" border="0" cellpadding="0" cellspacing="0" align="center">
                                                <tr>
                                                    <td class="no-bg hauto" height="30" align="center" valign="top" bgcolor="#af171c" style="height: 30px; background-repeat: no-repeat; background-position: top center; background-color:#af171c;">
                                                            <div>
                                                            </div>
                                                    </td>
                                                </tr>
                                                
                                            </table>
                                        </td>
                                    </tr>
                                    <tr>
                                        <!---->	
                                    </table><span style="display:none;"> </span>
                                    <!--  -->
                                    <!---->
                                    <table class="responsive" width="100%" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#EEEEEE" style="width:100%; min-width: 100%">
                                        <td align="center" valign="top">
                                            <table width="640" border="0" align="center" cellpadding="0" cellspacing="0" class="w85 cntr" id="module-wrapper">
                                                <tbody>
                                                    <tr>
                                                        <td class="mobile-module-spacers" height="40" style="height: 40" bgcolor="#EEEEEE"></td>
                                                    </tr>
                                                    <tr>
                                                        <td align="center" valign="top" bgcolor="#EEEEEE" style="background-color:#EEEEEE">
                                                            <table width="640" border="0" align="center" cellpadding="0" cellspacing="0" class="w100 cntr" id="module-hero-5A"  bgcolor="#EEEEEE" style="background-color:#EEEEEE;">
                                                                <tbody>
                                                                    <tr class="copyarea_show_copyareaonly featuretitleoption_hide_optional_above_show_copyareaonly para_spacer_hide_optional_show_copyareaonly_above">
                                                                        <td height="13" style="height:13px; line-height:13px; mso-line-height-rule:exactly; font-size: 0px; display: block">&nbsp;</td>
                                                                    </tr>
                                                                    <tr class="copyarea_show_copyareaonly featuretitleoption_hide_optional_above_show_copyareaonly para_spacer_hide_optional_show_copyareaonly_above">
                                                                        <td height="13" style="height:13px; line-height:13px; mso-line-height-rule:exactly; font-size: 0px; display: block">&nbsp;</td>
                                                                    </tr>
                                                                    <!-- -->
                                                                    <tr class="copyarea_show_copyareaonly">
                                                                        <td class="copyarea_show_copyareaonly_padding" align="center" valign="top" bgcolor="#FFFFFF" style="background-color:#FFFFFF; padding-bottom:10px;">
                                                                            
                                                                            <table width="560" border="0" cellspacing="0" cellpadding="0" align="center" class="w85 cntr">
                                                                                <tbody class="inline-block">
                                                                                    
                                                                                    <tr class="copyarea_show_copyareaonly featuretitleoption_hide_optional_above_show_copyareaonly para_spacer_hide_optional_show_copyareaonly_above" >
                                                                                        <td style="height:80px; line-height:13px; mso-line-height-rule:exactly; font-size: 0px; display: block; padding:10px;text-align: center;" >
                                                                                            <img width="360" src="https://satayo.cloud/images/logo.png" />
                                                                                        </td>
                                                                                    </tr>
                                                                                    <tr>
                                                                                        <td class="mobile-module-spacers" height="40" style="height: 40" bgcolor="#FFFFFF"></td>
                                                                                    </tr>
                                                                                    <!-- -->
                                                                                    <tr class="show_optional">
                                                                                        <td class="paragraph-mobile-css mobile-module-inner-spacing" align="**global.global-text-align{{3 valign="top" style="font-family:SST, SST-Roman, Arial, Helvetica, sans-serif; font-size: 14px; line-height: 24px; mso-line-height-rule:exactly; color: #363636; direction:ltr; padding-bottom:10px;" dir="ltr">
                                                                                            Ciao __,<br><br>In data 13 Ottobre 2024 <a href="https://satanyo.cloud/">Satayo</a> ha rilevato diversi accessi non autorizzati alla piattaforma,  
                                                                                            <br>L'account è stato di conseguenza disabilitato al fine di preservare gli accessi ai dati sensibili afferenti alla vostra azienda.<br>
                                                                                            <br><b>Per recuperare l'account occorre resettare le credenziali. ed effettuare un nuovo login mediante la form di login presente <a href="https://satanyo.cloud/">Satayo</a> o eseguire la procedure di recupero password utilizzando la medesima piattaforma <a href="https://satanyo.cloud/">Satayo</a></b>. 
                                                                                            <br><br>Ti verrà richiesto di consultare le sessioni attive e di validare e segnalare quelle che non risultano conosciute.  
                                                                                            <br><br>
                                                                                            <br><br>Nel ringraziarti per la collaborazione, 
                                                                                            <br>ti porgiamo i nostri più cordiali saluti.
                                                                                            <br><br>
                                                                                            <br><br>Lo staff di Satayo  

                                                                                        </td>
                                                                                    </tr>
                                                                                    <tr>
                                                                                        <td class="hide" height="13" style="height:*global.gbl-mb-module-outterSpacer-height**px; line-height:13px; mso-line-height-rule:exactly; font-size: 0px; display: block">&nbsp;</td>
                                                                                    </tr>
                                                                                </tbody>
                                                                            </table>
                                                                        </td>
                                                                    </tr>
                                                                    
                                                                    <!-- -->
                                                                </tbody>
                                                            </table>
                                                        </td>
                                                    </tr>
                                                </tbody>
                                            </table>
                                        </td>
                                    </tr>
                                </table>
                            </td>
                        </tr>
                    </table>
                    <!---->
                    <!--  --><span style="display:none;"> </span>
                    <!--  -->
                    <table class="responsive" width="100%" border="0" align="center" cellpadding="0" cellspacing="0" bgcolor="#FFFFFF" style="width:100%; min-width: 100%">
                        <tr>
                            <td align="center" valign="top">
                                <table id="footer-section" class="responsive" border="0" cellpadding="0" cellspacing="0" align="center" width="740">
                                    <tr>
                                        <td align="center" valign="top">
                                            <table class="responsive" bgcolor="#EEEEEE" border="0" cellpadding="0" cellspacing="0" align="center" width="740">
                                                <tr>
                                                    <td bgcolor="#EEEEEE" align="center" valign="top" style="padding-top: 20px; padding-bottom:20px">
                                                        <table class="responsiveInner" width="640" border="0" align="center" cellpadding="0" cellspacing="0">
                                                            <tbody>
                                                                <tr>
                                                                    <td align="center" valign="top">
                                                                        <!-- -->
                                                                        <table class="w95 cntr" cellpadding="0" cellspacing="0" border="0" width="640">
                                                                            <tr>
                                                                                <td align="center" valign="top" style="font-family: SST, SST-Roman, Arial, Helvetica, sans-serif; font-size: 11px; line-height: 21px; color: #999999; direction:ltr;" dir="ltr">
                                                                                    <!-- -->
                                                                                    <span style="direction:ltr" dir="ltr">&copy;2024 Satayo.</span>
                                                                                    <span style="height:15px; line-height:15px; mso-line-height-rule:exactly; display:block"><br><br></span>
                                                                                    SATAYO - dev by Würth Phoenix © All rights reserved.
                                                                                    <span style="height:15px; line-height:15px; mso-line-height-rule:exactly; display:block"><br><br></span>
                                                                                </td>
                                                                            </tr>
                                                                        </table>
                                                                    </td>
                                                                </tr>
                                                            </tbody>
                                                        </table>
                                                    </td>
                                                </tr>
                                            </table>
                                        </td>
                                    </tr>

                                </table>
                            </td>
                        </tr>
                    </table>
                </td>
            </tr>
        </table>
    </td>
</tr>
</table>
</center>
</body>
</html>
```

## CHALLENGE 1 ( UNLOCKED )

Please provide the name of the service the phishing campaign is targeting in such format `WPCTF{service_url}`

```
WPCTF{https://satayo.cloud/}
```

## CHALLENGE 2 ( LOCKED - NEED CHALLENGE 1 )

What is the email of the developer?

```
WPCTF{evil.evil.evil.evil.evil.999@protonmail.com}
```

## CHALLENGE 3 ( LOCKED - NEED CHALLENGE 2 )

What is the email of the threat actor? 

```
WPCTF{belzebuohm@gmail.com}
```

## CHALLENGE 4 ( LOCKED - NEED CHALLENGE 3 )

What event did the developer attend? In detail, what/who was the main attraction? 

```
WPCTF{John Maus}
```

## CHALLENGE 5 ( LOCKED - NEED CHALLENGE 4 )

What is the favorite game of the phishing kit developer?

```
WPCTF{DOOM}
```
