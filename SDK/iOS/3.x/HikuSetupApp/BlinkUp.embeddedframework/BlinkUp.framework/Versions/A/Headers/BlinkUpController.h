/******************************************************************************
 * - Copyright Electric Imp, Inc. 2013. All rights reserved.
 * - License: <#LICENSE#>
 *
 * <#SUMMARY INFORMATION#>
 */

#import <Foundation/Foundation.h>

@class BlinkUpController;
@class BlinkUpWifiConfig;
@class BlinkUpWPSConfig;
@class BlinkUpStringFormatParameters;

#pragma mark -
#pragma mark ** BlinkUpDelegate API **

@protocol BlinkUpDelegate <NSObject>

// all methods are optional
@optional

// This method fires when the actual flashing is done.  It does not imply success or failure of any kind.
- (void)blinkUp:(BlinkUpController *)blinkUpController
  flashCompleted:(BOOL)flashDidComplete;

//This method is called when a user presses the "cancel"  button in the navigation bar on the first
// view controller that is presented by blinkUp when using presentWifiSettings....
// This can be used to log a user out, or perform other cleanup actions when a user
//  exits the BlinkUp process from a point other than performing a blinkup.
//This method may also be called in the case of a fatal startup (example: no network connection)
- (void)blinkUpWillExitFromCancel:(BlinkUpController *)blinkUpController;

// This method fires when the Clear Wifi config flashing is done.  It does not imply success or failure of any kind.
- (void)blinkUpClearConfigComplete:(BlinkUpController *)blinkUpController;

// validated is called after the API key is validated and the app is ready to start the blink up process
- (void)blinkUp:(BlinkUpController *)blinkUpController
  keyValidated:(BOOL)apiKeyValidated
  error:(NSError *)err;

// This method will be called upon success, or upon timeout (set via agentUrlTimeout)
// verifiedDate is nil until the device successfully connects and downloads its firmware
// agentURL is nil until the proxy agent is ready
// impeeId is nil until the proxy agent is ready
- (void)blinkUp:(BlinkUpController *)blinkUpController
  statusVerified:(NSDate *)verifiedDate
  agentURL:(NSURL *)agentURL
  impeeId:(NSString *)impeeId
  error:(NSError *)error;
@end

#pragma mark -
#pragma mark ** BlinkUpDelegate API **

@interface BlinkUpController : NSObject

#pragma mark ** Designated Initializer **
// - (id)init; should be used as initializer

#pragma mark ** Standard and Custom **

//This property is usually set via a method call, such as present...
@property (nonatomic, weak) NSObject <BlinkUpDelegate> *delegate;

// After the BlinkUp is shown on the device, it will take the imp some time to connect
//  to the server and for the application to retrieve the AgentUrl. By default this time
//  period will last 60 seconds. If you wish to override the timeout period, this property
//  can be set with the number of seconds before the AgentUrl checking times out
// When the timeout occurs, the delegate method blinkUp:statusVerfied:agentUrl:impeeId:error
//  will be called with an error status
@property (nonatomic) NSTimeInterval agentUrlTimeout;

// The planId is used slightly differently in the standard and custom modes.
// In the standard and custom mode, the planId will be automatically generated and
//  can be retrieved by reading this property after the delegate method
//  blinkupStatusUpdate:verified:agentURL:error: sets validated=YES
// In order to set the planId in standard mode, do not set it here. Instead, use the
//  presentWifiSettings... method that includes the planId as a parameter
// In order to set the planId in custom mode, set this property before calling
//  aquireSetupToken.....
// If setting the planId property, it must be an existing Id previously generated by Electric Imp

// For an explanation of what the planId is or code chunks, check FAQ.markdown
@property (nonatomic, copy) NSString *planId;

// Before the BlinkUp flashes occur there is a countdown to allow the user to
// prepare the device. The preFlashCountdownTime is the number of seconds that
// the countdown occurs for. The default is 3 seconds. This property cannot be
// set lower than 3 seconds, and not more than 10.
@property (nonatomic, assign) NSInteger preFlashCountdownTime;

/*!
 *  @brief  Brightness of the screen during the flash
 *
 *  A value from 0 ... 1 indicating brightness of the screen from 0 (darkest)
 *  to 1 (brightest). If your sensor is running hot, you can lower this number
 *
 *  Default value is 0.8
 */
@property (nonatomic, assign) float screenBrightness;

// Stop the current status verification.
// This will stop the SDK from polling and prevents the
// blinkupStatusVerified:agentURL:impeeId:error: delegate callbacks from being sent.
// Typically done after the end user has blinked up and is waiting for status
// verification and the end user hits some kind of cancel or back button
// because the don't want to wait for verification to complete.
- (void)stopCurrentStatusVerification;

// Removes the BlinkUp from the screen.
// This method should only be used in failed states and should not be a part of the
// normal BlinkUp process. It does not stop any background status verifications.
- (void)forceDismissBlinkUpController;

// Can be used to clear cached and persistent data (saved passwords, etc.)
+ (void)clearSavedData;

// When the BlinkUp network selection screens are initially presented, it is possible that they
// can not be shown, most often caused by no internet connection. In these cases an alert view
// can be shown to the user to inform them there was an issue.
//
// If you would like to know the specific error, it can be seen from the
// blinkUpController:keyValidated:error delegate method.
// The blinkUpwilExitFromCancel delegate method is also called on the fatal failure
//
// Defaults to true
@property (nonatomic) BOOL shouldPresentAlertViewOnFatalError;


#pragma mark ** Standard Options **

// if you want to use an existing PlanID, the BlinkUpController's planId property must be set.
// If planId is nil (or not set), this method will automatically fetch a new planId.
// Most developers won't need to reuse existing planIDs.  Please contact support if you have questions.
- (NSError *)presentWifiSettingsWithDelegate:(NSObject <BlinkUpDelegate> *)delegate
  APIKey:(NSString *)apiKey
  animated:(BOOL)animated;

#pragma mark ** Standard Option Limited Customization **

///////
// Global
@property (nonatomic, assign) BOOL hideStatusBar;

// Most visible strings in the BlinkUp interface can be changed via the localization file.
//


// On occasion it may be useful to change or append to it additional information about the
//  state of the application for the user (such as the device they are about to BlinkUp,
//  or their username). By addiing objects into the various stringParams arrays it is possible
//  to inject dynamic content using standard stringWithFormat notation in the localized file string
//  for example "This is my footer with %@ parameter"
@property (nonatomic, strong, readonly) BlinkUpStringFormatParameters *stringParams;

///////
// Pre-flash interstial
// image should be 280x380 pixels @1x and 560x760 pixels @2x (retina)
@property (nonatomic, strong) UIImage *interstitialImage;

// For added security, setting this flag to true will not allow users to see the ssid passwords
// that they are entering into the software
@property (nonatomic, assign) BOOL disableWifiDetailShowPassword;

#pragma mark ** Advanced UI Customization API **

// setup token acqusition
// if you want to use an existing PlanID, the BlinkUpController's planId property must be set.
// If planId is nil (or not set), this method will automatically fetch a new planId.
// Most developers won't need to reuse existing planIDs.  Please contact support if you have questions.
- (void)acquireSetupTokenWithAPIKey:(NSString *)apiKey
  completionHandler:(void (^)(BOOL didSucceed))outerCompletionHandler;

// flashing
// before calling a presentFlashWithDelegate method, you must first call acquireSetupTokenWithAPIKey:completionHandler:

- (NSError *)presentFlashWithDelegate:(NSObject <BlinkUpDelegate> *)delegate
  wifiConfig:(BlinkUpWifiConfig *)wifiConfig
  APIKey:(NSString *)apiKey
  animated:(BOOL)animated;

- (NSError *)presentFlashWithDelegate:(NSObject <BlinkUpDelegate> *)delegate
  wpsConfig:(BlinkUpWPSConfig *)wpsConfig
  APIKey:(NSString *)apiKey
  animated:(BOOL)animated;

// There are no prerequisites for presentClearDeviceFlashWithDelegate:animated:

- (NSError *)presentClearDeviceFlashWithDelegate:(NSObject <BlinkUpDelegate> *)delegate
  animated:(BOOL)animated;

#pragma mark ** Non BlinkUp Method API **

/**
 *  Retrieve a single use setup token that can be used by an Impee if it is being
 *  configured using a method other than BlinkUp
 *
 *  If you want to use an existing PlanID, the BlinkUpController's planId property must be set.
 *  If planId is nil (or not set), this method will automatically fetch a new planId.
 *  Most developers won't need to reuse existing planIDs.  Please contact support if you have questions.
 *
 *  @param apiKey            API key assigned to you by Electric Imp
 *  @param completionHandler A block to handle the sucess or failure of token retrieval
 */
- (void)singleUseTokenWithAPIKey:(NSString *)apiKey
  completionHandler:(void (^)(BOOL didSucceed, NSString *token))completionHandler;


/**
 *  Query the Electric Imp server to see if the token has been claimed
 *
 *  The delegate method blinkUp:statusVerified:agentURL:impeeId:error will be called
 *  with the results of the query
 *
 *  @param token  The one time use token that was used by the impee
 *  @param apiKey API key assigned to you by Electric Imp
 */
- (void)validateSingleUseToken:(NSString *)token apiKey:(NSString *)apiKey;


/**
 *  Present the user with an interface to enter Wifi information. This method is used
 *  when an external communication method is used to set the wifi information on the imp.
 *  None of the delegate methods will be called when using this method
 *
 *  @param animated          Animate the presentation
 *  @param completionHandler The completion handler is executed immediately before control
 *    is returned to your program. Please check all inputs as they are all exclusive options
 *    that is set based on the users actions.
 */
- (void)presentWifiSettingsForExternalConfigurationAnimated:(BOOL)animated completionHandler:(void (^)(BlinkUpWifiConfig *wifiConfig, BlinkUpWPSConfig *wpsConfig, BOOL clearConfiguration, BOOL userDidCancel))completionHandler;

@end


//If you implement the deprecated methods, please update your code
#import <BlinkUp/BlinkUpController_Deprecated.h>
