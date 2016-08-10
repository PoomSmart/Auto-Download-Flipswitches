#import "../Common.h"

@interface AutoDownloadUpdatesSwitch : NSObject <FSSwitchDataSource>
@end

extern CFStringRef kSSUserDefaultsKeyAutoUpdatesEnabled;
extern CFStringRef kSSUserDefaultsIdentifier;

@implementation AutoDownloadUpdatesSwitch

- (FSSwitchState)stateForSwitchIdentifier:(NSString *)switchIdentifier
{
	Boolean keyExist;
	Boolean enabled = CFPreferencesGetAppBooleanValue(kSSUserDefaultsKeyAutoUpdatesEnabled, kSSUserDefaultsIdentifier, &keyExist);
	if (!keyExist)
		return FSSwitchStateOn;
	return enabled ? FSSwitchStateOn : FSSwitchStateOff;
}

- (void)applyState:(FSSwitchState)newState forSwitchIdentifier:(NSString *)switchIdentifier
{
	if (newState == FSSwitchStateIndeterminate)
		return;
	CFBooleanRef enabled = newState == FSSwitchStateOn ? kCFBooleanTrue : kCFBooleanFalse;
	CFPreferencesSetAppValue(kSSUserDefaultsKeyAutoUpdatesEnabled, enabled, kSSUserDefaultsIdentifier);
	CFPreferencesAppSynchronize(kSSUserDefaultsIdentifier);
	SSSoftwareUpdatesStore *store = [[SSSoftwareUpdatesStore alloc] init];
	[store hideApplicationBadgeForPendingUpdates];
	if (enabled == kCFBooleanTrue) {
		[store reloadFromServerWithCompletionBlock:^(void *block, NSArray *array, NSError *error) {}];
	}
	[store release];
}

@end