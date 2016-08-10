#import <Preferences/PSSpecifier.h>
#import <dlfcn.h>
#import <substrate.h>
#import "../PS.h"
#import <Flipswitch/FSSwitchPanel.h>
#import <Flipswitch/FSSwitchDataSource.h>

@interface SSDevice : NSObject
+ (SSDevice *)currentDevice;
@property(readonly) NSString *userAgent;
@property(readonly) NSSet *automaticDownloadKinds;
- (void)unionAutomaticDownloadKinds:(NSSet *)set withCompletionBlock:(id)completion;
- (void)minusAutomaticDownloadKinds:(NSSet *)set withCompletionBlock:(id)completion;
@end

@interface SSSoftwareUpdatesStore : NSObject
- (void)hideApplicationBadgeForPendingUpdates;
- (void)reloadFromServerWithCompletionBlock:(void (^)(void *block, NSArray *array, NSError *error))completionBlock;
@end

@interface SSURLBagContext : NSObject
+ (SSURLBagContext *)contextWithBagType:(long long)type;
@property(retain) NSNumber *userIdentifier;
- (void)setValue:(id)value forHTTPHeaderField:(NSString *)header;
@end

@interface SSAccount : NSObject
@property(retain) NSNumber *uniqueIdentifier;
@end

@interface SSAccountStore : NSObject
+ (SSAccountStore *)defaultStore;
- (SSAccount *)activeAccount;
@end

@interface ISURLBag : NSObject
- (id)valueForKey:(NSString *)key;
@end

@interface ISURLBagCache : NSObject
+ (ISURLBagCache *)sharedCache;
- (ISURLBag *)URLBagForContext:(SSURLBagContext *)context;
@end

#ifdef AUTO_DOWNLOADS

extern NSString *SSDeviceAutomaticDownloadKindsChangedNotification;
extern NSString *SSHTTPHeaderUserAgent;
extern NSString *SSHTTPHeaderXAppleStoreFront;
extern "C" id SSVStoreFrontIdentifierForAccount(SSAccount *);

static SSURLBagContext *bagContext()
{
	SSURLBagContext *context = [SSURLBagContext contextWithBagType:0];
	SSAccountStore *store = [SSAccountStore defaultStore];
	SSAccount *account = [store activeAccount];
	SSDevice *device = [SSDevice currentDevice];
	NSString *userAgent = device.userAgent;
	[context setValue:userAgent forHTTPHeaderField:SSHTTPHeaderUserAgent];
	id storeFrontIdentifier = SSVStoreFrontIdentifierForAccount(account);
	[context setValue:storeFrontIdentifier forHTTPHeaderField:SSHTTPHeaderXAppleStoreFront];
	context.userIdentifier = account.uniqueIdentifier;
	return context;
}

static NSArray *_configurations = nil;

static void reloadAutomaticDownloadConfigurations()
{
	SSURLBagContext *context = bagContext();
	ISURLBagCache *cache = [ISURLBagCache sharedCache];
	ISURLBag *bag = [cache URLBagForContext:context];
	if (bag) {
		NSDictionary *downloads = nil;
		downloads = [bag valueForKey:@"automatic-downloads2"];
		if (downloads == nil)
			downloads = [bag valueForKey:@"automatic-downloads"];
		_configurations = [downloads objectForKey:@"configurations"];
		NSLog(@"%@", _configurations);
		[_configurations retain];
		CFPreferencesSetAppValue(CFSTR("auto-download-configurations"), _configurations, CFSTR("com.apple.mobilestoresettings"));
		CFPreferencesAppSynchronize(CFSTR("com.apple.mobilestoresettings"));
	} else
		_configurations = (NSArray *)CFPreferencesCopyAppValue(CFSTR("auto-download-configurations"), CFSTR("com.apple.mobilestoresettings"));
}

static NSArray *automaticDownloadConfigurations()
{
	reloadAutomaticDownloadConfigurations();
	return _configurations;
}

static NSSet *activeAutomaticDownloadKinds()
{
	SSDevice *device = [SSDevice currentDevice];
	return device.automaticDownloadKinds;
}

static NSSet *ssDownloadKindsForCanonicalName(NSString *name)
{
	NSArray *configurations = automaticDownloadConfigurations();
	NSUInteger index = NSNotFound;
	for (NSUInteger idx = 0; idx < configurations.count; idx++) {
		NSDictionary *value = configurations[idx];
		if ([value[@"canonical-name"] isEqualToString:name]) {
			index = idx;
			break;
		}
	}
	if (index == NSNotFound)
		return nil;
	NSDictionary *configuration = configurations[index][name];
	NSArray *mediaTypes = configuration[@"media-types"];
	return [NSSet setWithArray:mediaTypes];
}

static BOOL automaticDownloadsEnabledForCanonicalName(NSString *name)
{
	return [ssDownloadKindsForCanonicalName(name) isSubsetOfSet:activeAutomaticDownloadKinds()];
}

static void setAutomaticDownloadsEnabledForCanonicalName(FSSwitchState newState, NSString *name)
{
	NSSet *set = ssDownloadKindsForCanonicalName(name);
	SSDevice *device = [SSDevice currentDevice];
	BOOL value = newState == FSSwitchStateOn;
	if (value)
		[device unionAutomaticDownloadKinds:set withCompletionBlock:NULL];
	else
		[device minusAutomaticDownloadKinds:set withCompletionBlock:NULL];
	reloadAutomaticDownloadConfigurations();
}

#endif