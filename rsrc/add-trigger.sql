create or replace function notify_of_new_blasts() returns trigger as $$
  begin
    notify newblast;
    return NEW;
  end
$$ language plpgsql;

drop trigger if exists notify_of_new_blasts on bugle_blast;

create trigger notify_of_new_blasts
  before insert on bugle_blast
  for each row execute procedure notify_of_new_blasts();
