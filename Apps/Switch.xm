#define AUTO_DOWNLOADS
#import "../Common.h"

@interface AutoDownloadAppsSwitch : NSObject <FSSwitchDataSource>
@end

@implementation AutoDownloadAppsSwitch

- (FSSwitchState)stateForSwitchIdentifier:(NSString *)switchIdentifier
{
	BOOL enabled = automaticDownloadsEnabledForCanonicalName(@"AUTO_DOWNLOAD_APPS");
	return enabled ? FSSwitchStateOn : FSSwitchStateOff;
}

- (void)applyState:(FSSwitchState)newState forSwitchIdentifier:(NSString *)switchIdentifier
{
	if (newState == FSSwitchStateIndeterminate)
		return;
	setAutomaticDownloadsEnabledForCanonicalName(newState, @"AUTO_DOWNLOAD_APPS");
}

@end